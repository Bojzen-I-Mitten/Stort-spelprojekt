// This is the main DLL file.

#pragma unmanaged

#include "object/GameObjectManager.h"
#include <thomas\ThomasCore.h>
#include <thomas\WindowManager.h>
#include <thomas\ThomasTime.h>
#include <thomas\graphics\Renderer.h>
#include <thomas\editor\gizmos\Gizmos.h>
#include <thomas\Physics.h>
#include <thomas\editor\Editor.h>
#include <thomas\editor\EditorCamera.h>
#include <thomas\utils\AutoProfile.h>
#include <thomas\utils\ProfileManager.h>
#include <thomas\utils\GpuProfiler.h>
#include <thomas\object\component\Camera.h>
#include <thomas\object\component\RenderComponent.h>
#include <thomas\graphics\LightManager.h>
#pragma managed
#include "ThomasManaged.h"
#include "resource\Model.h"
#include "resource\Resources.h"
#include "object\Component.h"
#include "object\component\physics\Rigidbody.h"
#include "ScriptingManager.h"
#include "ThomasSelection.h"
#include "GUI\editor\Gizmos.h"
#include "object\GameObject.h"
#include "Debug.h"
#include "system/SceneManager.h"

using namespace thomas;

namespace ThomasEngine {
	
	ThomasWrapper^ ThomasWrapper::Thomas::get()
	{
		return s_SYS; // Access thomas runtime system.
	}
	Scene^ ThomasWrapper::CurrentScene::get()
	{
		return s_SYS->m_scene->CurrentScene;
	}
	SceneManager^ ThomasWrapper::SceneManagerRef::get()
	{
		return s_SYS->m_scene;
	}

	void ThomasWrapper::Start()
	{
		Start(true);
	}
	void ThomasWrapper::Start(bool editor) 
	{
		inEditor = editor;
		//Thread init
		Thread::CurrentThread->Name = "Main Thread";
		mainThreadDispatcher = System::Windows::Threading::Dispatcher::CurrentDispatcher;					// Create/Get dispatcher
		// System initialization
		String^ enginePath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);
		Environment::SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget::User);

		// Thomas Initialization
		Thomas->m_scene = gcnew SceneManager();
		s_Selection = gcnew ThomasSelection();
		s_GameObjectManager = gcnew GameObjectManager();
		thomas::ThomasCore::Core().registerThread();
		thomas::ThomasCore::Init();


		utils::profiling::ProfileManager::resetFrameCounter();

		if (ThomasCore::Initialized())
		{
			
			
			Component::LoadExternalComponents();

			RenderFinished = gcnew ManualResetEvent(true);
			UpdateFinished = gcnew ManualResetEvent(false);
			StateCommandProcessed = gcnew ManualResetEvent(false);
			Thomas->m_scene->LogicThreadClearScene();
#if _EDITOR

			if (InEditor()) {
				Model::InitPrimitives();
				Resources::LoadAll(Application::editorAssets);
				ScriptingManager::Init();
			}
			else
			{
				Resources::LoadAll(Application::editorAssets + "\\FXIncludes");
			}

			
#endif

			LOG("Thomas fully initiated, Chugga-chugga-whoo-whoo!");
			logicThread = gcnew Thread(gcnew ThreadStart(StartEngine));
			logicThread->Name = "Thomas Engine (Logic Thread)";
			logicThread->Start();

			renderThread = gcnew Thread(gcnew ThreadStart(StartRenderer));
		
			renderThread->Name = "Thomas Engine (Render Thread)";
			renderThread->Start();

		}
	}
	void ThomasWrapper::SampleRam(System::Object^ stateInfo)
	{
#ifdef BENCHMARK
		float ramUsage = float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 * 0.001f * 0.001f);
		utils::profiling::ProfileManager::setRAMUsage(ramUsage);

		utils::profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
		utils::profiling::ProfileManager::setVRAMUsage(profiler->GetMemoryUsage(), profiler->GetTotalMemory());
#endif

	}

	void ThomasWrapper::MainThreadUpdate()
	{
#ifdef _EDITOR
		if (!playing)
			ScriptingManager::ReloadAssembly(false);
#endif
		constexpr uint32_t RAM_SAMPLE_FRAME = 1000;
		static uint32_t WaitSampleFrame = 0;
		if (WaitSampleFrame++ > RAM_SAMPLE_FRAME)
		{
			ThreadPool::QueueUserWorkItem(gcnew WaitCallback(SampleRam));
			WaitSampleFrame = 0;
		}
	}


	void ThomasWrapper::StartRenderer()
	{
		// Render thread start

		ThomasCore::Core().registerThread();
		while (ThomasCore::Initialized())
		{
			float timeStart = ThomasTime::GetElapsedTime();
			PROFILE("StartRenderer")
			{
				PROFILE("StartRenderer - Wait")
				UpdateFinished->WaitOne();
			}
			UpdateFinished->Reset();
			ThomasCore::Render();
			RenderFinished->Set();
			renderTime = ThomasTime::GetElapsedTime() - timeStart;

			float gpuTime = utils::D3D::Instance()->GetProfiler()->GetFrameTime() * 1000.0f * 1000.0f * 1000.0f;
			utils::profiling::ProfileManager::storeGpuSample((long long)gpuTime);

#ifdef BENCHMARK
			utils::profiling::ProfileManager::newFrame();
#endif
		}
	}

	void ThomasWrapper::SynchronousExecution()
	{
		// Process state switch commands
		ProcessCommand();

		// Refresh frame
		thomas::graphics::LightManager::Update();
		CopyCommandList();

		// Enter async. state 
#ifdef _EDITOR
					// This is only relevant if we are running with editor, should be removed when build
		for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
		{
			GameObject^ gameObject = CurrentScene->GameObjects[i];

			if (gameObject->MoveStaticGroup())
			{
				// Fetch the adress of where an object might be moved to
				thomas::object::Object* new_temp = CurrentScene->GameObjects[i]->nativePtr;

				// Fetch the adress of the object that might be moved
				thomas::object::Object* old_native = gameObject->moveStaticGroup();

				// Find the wrapped gameobject of the object that might be moved
				GameObject^ temp = GameObject::FindGameObjectFromNativePtr(static_cast<thomas::object::GameObject*>(old_native));

				if (temp) // If temp is nullptr, no managed object has been invalidated, no move will be done.
					temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.
			}

			else if (gameObject->MakeStatic())
			{
				thomas::object::Object* new_temp = CurrentScene->GameObjects[i]->nativePtr;

				thomas::object::Object* old_native = gameObject->setStatic();

				GameObject^ temp = GameObject::FindGameObjectFromNativePtr(static_cast<thomas::object::GameObject*>(old_native));

				if (temp) // If temp is nullptr, no managed object has been invalidated.
					temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.

			}

			else if (gameObject->MakeDynamic())
			{
				thomas::object::Object* new_temp = CurrentScene->GameObjects[i]->nativePtr;

				thomas::object::Object* old_native = gameObject->setDynamic();

				GameObject^ temp = GameObject::FindGameObjectFromNativePtr(static_cast<thomas::object::GameObject*>(old_native));

				if (temp) // If temp is nullptr, no managed object has been invalidated.
					temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.
			}

		}
#endif
	}
	
	void ThomasWrapper::CopyCommandList()
	{
		utils::profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
		if (showStatistics)
		{
			ImGui::Begin("Statistics", &(bool&)showStatistics, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("%d FPS (%.2f ms)", ThomasTime::GetFPS(), ThomasTime::GetFrameTime());
			ImGui::Text("Logic Thread: %.02f ms	 Render Thread: %.02f ms", logicTime*1000.0f, renderTime*1000.0f);
			ImGui::Text("CPU: %.02f ms	GPU: %.02f ms", (logicTime + renderTime)*1000.0f, profiler->GetFrameTime()*1000.0f);
			ImGui::Text("Draw calls: %d	Verts: %d", profiler->GetNumberOfDrawCalls(), profiler->GetVertexCount());
			ImGui::Text("VRAM Usage: %.2f MB (of %.2f MB)", profiler->GetMemoryUsage(), profiler->GetTotalMemory());
			ImGui::Text("RAM Usage: %.2f MB", 0.0f);
			ImGui::Text("Draw time: %0.2f ms", profiler->GetDrawTotal()*1000.0f);
			ImGui::Text("	Window clear: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_MAIN_CLEAR)*1000.0f);
			ImGui::Text("	Main objects: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_MAIN_OBJECTS)*1000.0f);
			ImGui::Text("	Particles: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_PARTICLES)*1000.0f);
			ImGui::Text("	Gizmo objects: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_GIZMO_OBJECTS)*1000.0f);
			ImGui::End();
		}

		if(WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->Initialized())
			WindowManager::Instance()->GetEditorWindow()->EndFrame(true);

		// Swap command lists
		thomas::graphics::Renderer::Instance()->TransferCommandList();
		thomas::editor::Gizmos::Gizmo().TransferGizmoCommands();
		thomas::graphics::Renderer::Instance()->ClearCommands();
		thomas::editor::Gizmos::Gizmo().ClearGizmos();

		//editor::EditorCamera::Instance()->GetCamera()->CopyFrameData();
//#ifdef _EDITOR
//		editor::Editor::GetEditor().Camera()->GetCamera()->CopyFrameData();
//#endif
	}

	void TriggerLoad()
	{
		// Called when system can replace 
	}

	void ThomasWrapper::StartEngine()
	{
		// Update thread start

		ThomasCore::Core().registerThread();
		while (ThomasCore::Initialized())
		{
			PROFILE("StartEngine")
			// Load scene
			if (Thomas->m_scene->LoadThreadWaiting())
			{
				// Swap scene
				RenderFinished->WaitOne();
				Thomas->m_scene->ListenToLoadProcess();
			}
			if (Thomas->m_scene->NoSceneExist())	// Wait for scene load
			{
				Thread::Sleep(500);
				continue;
			}
			float timeStart = ThomasTime::GetElapsedTime();
			//PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic);
			Object^ lock = CurrentScene->GetGameObjectsLock();
			try {
				bool hasKey = thomas::WindowManager::Instance() && thomas::WindowManager::Instance()->GetGameInput()->GetLastKey() != NULL;
				thomas::ThomasTime::Update();
				
				if (WindowManager::Instance()->WaitingForUpdate()) //Make sure that we are not rendering when resizing the window.
					RenderFinished->WaitOne();
				WindowManager::Instance()->Update();


				ThomasCore::Update();
				Monitor::Enter(lock);

				CurrentScene->InitGameObjects(IsPlaying());

				

				//Logic
				{
					PROFILE("LogicLoop")
					for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
					{

						GameObject^ gameObject = CurrentScene->GameObjects[i];
						if (gameObject->GetActive())
						{
							gameObject->Update();
						}
					}
				}
				editor::EditorCamera::Instance()->Update();


				if (IsPlaying())
				{
					//Physics
					{
						PROFILE("UpdateRigidbodies")

						thomas::Physics::UpdateRigidbodies();
					}
					for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ gameObject = CurrentScene->GameObjects[i];
						if (gameObject->GetActive())
							gameObject->FixedUpdate(); //Should only be ran at fixed timeSteps.
					}
					thomas::Physics::Simulate();
				}


				//Rendering
				if (WindowManager::Instance())
				{
					// Editor render
					if (WindowManager::Instance()->GetEditorWindow() && RenderEditor)
					{
						editor::EditorCamera::Instance()->Render();
						//GUI::ImguiStringUpdate(thomas::ThomasTime::GetFPS().ToString(), Vector2(Window::GetEditorWindow()->GetWidth() - 100, 0)); TEMP FPS stuff :)
						for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
						{
							GameObject^ gameObject = CurrentScene->GameObjects[i];
							if (gameObject->GetActive())
								gameObject->RenderGizmos();
						}

							s_Selection->render();
					}	//end editor rendering
				
					

					for (object::component::Camera* camera : graphics::Renderer::Instance()->getCameraList().getCameras())
					{
						camera->Render();
					}
					if (WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->GetInput()->GetKeyDown(Keys::F1)) {
						showStatistics = !showStatistics;
					}
					if(hasKey)
						thomas::WindowManager::Instance()->GetGameInput()->SetLastKey(NULL);
				}
			}
			catch (Exception^ e) {
				Debug::LogException(e);
				/*if (playing && inEditor)
					IssueStopPlay();*/
			}
			finally
			{
				logicTime = ThomasTime::GetElapsedTime() - timeStart;
				if (WindowManager::Instance())
				{
					thomas::object::component::RenderComponent::ClearList();

					// Wait for renderer
					{
						PROFILE("StartEngine - Wait");
						RenderFinished->WaitOne();
					}
					/* Render & Update is synced.
					*/
					SynchronousExecution();
					RenderFinished->Reset();
					UpdateFinished->Set();
				}

				Monitor::Exit(lock);
				mainThreadDispatcher->BeginInvoke(
					System::Windows::Threading::DispatcherPriority::Normal,
					gcnew MainThreadDelegate(MainThreadUpdate));
			}

			

		}
		// Initiate log dump


		// Shutdown
		WaitLogOutput = gcnew ManualResetEvent(false);
		renderThread->Join();	// Wait until thread is finished
		WaitCallback^ logOut = gcnew WaitCallback(DumpProfilerLog);
		ThreadPool::QueueUserWorkItem(logOut);
#ifdef BENCHMARK
		WaitLogOutput->WaitOne();
#endif


		Resources::UnloadAll();
		ThomasCore::Destroy();
	}

	void ThomasWrapper::DumpProfilerLog(System::Object^ stateInfo)
	{
#ifdef BENCHMARK
		utils::profiling::ProfileManager::dumpDataToFile();
#endif
		WaitLogOutput->Set();
	}

	void ThomasWrapper::Exit() 
	{
		thomas::ThomasCore::Exit();
	}

	void ThomasWrapper::CreateThomasWindow(IntPtr hWnd, bool isEditor)
	{
		if (thomas::ThomasCore::Initialized())
			WindowManager::Instance()->Create((HWND)hWnd.ToPointer(), isEditor);

		if (isEditor) {
			inEditor = true;
			thomas::ThomasCore::SetEditor(true);
		}
			
	}


	void ThomasWrapper::eventHandler(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam) {
		thomas::Window::EventHandler((HWND)hWnd.ToPointer(), msg, (WPARAM)wParam.ToPointer(), (LPARAM)lParam.ToPointer());
	}

	void ThomasWrapper::Resize(IntPtr hWnd, double width, double height)
	{
		Window* window = WindowManager::Instance()->GetWindow((HWND)hWnd.ToPointer());
		if (window)
			window->QueueResize();
	}

	void ThomasWrapper::Update()
	{
		Debug::UpdateCoreLog();
		if (editor::EditorCamera::Instance()->HasSelectionChanged())
			s_Selection->UpdateSelectedObjects();
	}

	void ThomasWrapper::ProcessCommand()
	{
		if (IssuedStateCommand)
		{
			Monitor::Enter(StateCommandProcessed);
			if (IssuedStateCommand == ThomasStateCommand::PlayIssued)
			{
				Play();
			}
			else if (IssuedStateCommand == ThomasStateCommand::StopIssued)
			{
				StopPlay();
			}
			else if (IssuedStateCommand == ThomasStateCommand::RestartIssued)
			{
				LOG("Entered restart function");
				StopPlay();
				Play();
			}
			IssuedStateCommand = ThomasStateCommand::NoCommand;
			StateCommandProcessed->Set();
			Monitor::Exit(StateCommandProcessed);
		}
	}

	void ThomasWrapper::Play()
	{
		while (Thomas->SceneManagerRef->IsAsyncLoading())
		{
			Thomas->SceneManagerRef->ListenToLoadProcess();
			Thread::Sleep(50);
		}
		thomas::ThomasCore::Core().OnPlay();
		ThomasEngine::Resources::OnPlay();
		CurrentScene->OnPlay();
		playing = RunningState::Running;
		Debug::Log("Running...");
	}
		
	Guid selectedGUID;
	void ThomasWrapper::StopPlay()
	{
		
		playing = RunningState::UnInitialized;
		// Synced state
		thomas::ThomasCore::Core().OnStop();

		if (s_Selection->Count > 0)
			selectedGUID = s_Selection[0]->m_guid;
		else
			selectedGUID = Guid::Empty;
		Thomas->m_scene->RestartCurrentScene();		// Reload temporary scene file.
		ThomasEngine::Resources::OnStop();
		if (selectedGUID != Guid::Empty)
		{
			GameObject^ gObj = (GameObject^)ThomasEngine::Object::Find(selectedGUID);
			if (gObj)
				s_Selection->SelectGameObject(gObj);
		}

		Debug::Log("Stopped...");
		playing = RunningState::Editor;
	}
	void ThomasWrapper::IssueStateCommand(ThomasStateCommand state)
	{
		Monitor::Enter(StateCommandProcessed);
		StateCommandProcessed->Reset();
		IssuedStateCommand = state;
		Monitor::Exit(StateCommandProcessed);
		StateCommandProcessed->WaitOne();
	}
	void ThomasWrapper::IssuePlay()
	{
		IssueStateCommand(ThomasStateCommand::PlayIssued);
	}

	void ThomasWrapper::IssueStopPlay()
	{
		IssueStateCommand(ThomasStateCommand::StopIssued);
	}

	void ThomasWrapper::IssueRestart()
	{
		IssueStateCommand(ThomasStateCommand::RestartIssued);
		//IssueStateCommand(ThomasStateCommand::StopIssued);
	}

	bool ThomasWrapper::IsPlaying()
	{
		return playing == RunningState::Running;
	}

	bool ThomasWrapper::RenderPhysicsDebug::get() { return thomas::Physics::Physics::s_drawDebug; }

	void ThomasWrapper::RenderPhysicsDebug::set(bool value) { thomas::Physics::Physics::s_drawDebug = value; }

	float ThomasWrapper::FrameRate::get() { return float(thomas::ThomasTime::GetFPS()); }

	void ThomasWrapper::SetEditorGizmoManipulatorOperation(ManipulatorOperation op)
	{
		thomas::editor::EditorCamera::Instance()->SetManipulatorOperation((ImGuizmo::OPERATION)op);
	}

	ThomasWrapper::ManipulatorOperation ThomasWrapper::GetEditorGizmoManipulatorOperation()
	{
		return (ManipulatorOperation)thomas::editor::EditorCamera::Instance()->GetManipulatorOperation();
	}

	void ThomasWrapper::ToggleEditorGizmoManipulatorMode()
	{
		thomas::editor::EditorCamera::Instance()->ToggleManipulatorMode();
	}

	
	bool ThomasWrapper::InEditor()
	{
		return inEditor;
	}

}