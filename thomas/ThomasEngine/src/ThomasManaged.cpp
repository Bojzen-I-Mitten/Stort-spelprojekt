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
#include "system/command/CommandQueue.h"

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

	ThomasWrapper::ThomasWrapper()
	{
		// Thomas Object Initiation
		m_scene = gcnew SceneManager();
		m_engineCommands = gcnew CommandQueue();
		m_sceneLock = gcnew Object();
	}
	ThomasWrapper::~ThomasWrapper()
	{
		m_scene->CurrentScene->UnLoad();
		Resources::UnloadAll();
		ThomasCore::Destroy();				// Delete core last
	}
	void ThomasWrapper::Start(bool editor) 
	{
		inEditor = editor;
		EditorWindowLoaded = gcnew ManualResetEvent(!inEditor);
		GameWindowLoaded = gcnew ManualResetEvent(!inEditor);
		//Thread init
		Thread::CurrentThread->Name = "Main Thread";
		mainThreadDispatcher = System::Windows::Threading::Dispatcher::CurrentDispatcher;					// Create/Get dispatcher
		// System initialization
		String^ enginePath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);
		Environment::SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget::User);

		// Thomas Initialization
		s_SYS = gcnew ThomasWrapper();
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

			LOG("Thomas fully initiated, Chugga-chugga-whoo-whoo!");
			logicThread = gcnew Thread(gcnew ThreadStart(StartEngine));
			logicThread->Name = "Thomas Engine (Logic Thread)";
			logicThread->Start();
			
		}
	}
#ifdef _EDITOR
	void ThomasWrapper::ThomasGameWindowLoaded()
	{
		GameWindowLoaded->Set();
	}

	void ThomasWrapper::ThomasEditorWindowLoaded()
	{
		EditorWindowLoaded->Set();
	}
#endif

	void ThomasWrapper::SampleRam(System::Object^ stateInfo)
	{
#ifdef MEMORY
		float ramUsage = float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 / 1024.0f / 1024.0f);
		utils::profiling::ProfileManager::setRAMUsage(ramUsage);


		utils::profiling::ProfileManager::setVRAMUsage(utils::profiling::GpuProfiler::Instance()->GetCurrentMemory());
#endif

	}

	void ThomasWrapper::MainThreadUpdate()
	{
#ifdef _EDITOR
		// Check if scripts require a reload
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
		GameWindowLoaded->WaitOne();
		EditorWindowLoaded->WaitOne();
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
#ifdef PERFORMANCE
			float gpuTime = utils::profiling::GpuProfiler::Instance()->GetFrameTime() * 1000.0f * 1000.0f * 1000.0f;
			utils::profiling::ProfileManager::storeGpuSample((long long)gpuTime);


			utils::profiling::ProfileManager::newFrame();
#endif
		}
	}

	void ThomasWrapper::SynchronousExecution()
	{
		Thomas->m_engineCommands->execute();	// Execute editor commands
		CurrentScene->SyncScene();				// Execute scene commands
		ProcessStateCommand();						// Process engine state switch commands

		// Refresh frame
		thomas::graphics::LightManager::Update();
		CopyCommandList();

		// Enter async. state 
#ifdef _EDITOR
					// This is only relevant if we are running with editor, should be removed when build
		//for each(GameObject^ gameObject in CurrentScene->GameObjects)
		//{
		//	if (gameObject->MoveStaticGroup())
		//	{
		//		// Fetch the adress of where an object might be moved to
		//		thomas::object::Object* new_temp = gameObject->nativePtr;

		//		// Fetch the adress of the object that might be moved
		//		thomas::object::Object* old_native = gameObject->moveStaticGroup();

		//		// Find the wrapped gameobject of the object that might be moved
		//		GameObject^ temp = CurrentScene->Find(static_cast<thomas::object::GameObject*>(old_native));

		//		if (temp) // If temp is nullptr, no managed object has been invalidated, no move will be done.
		//			temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.
		//	}

		//	else if (gameObject->MakeStatic())
		//	{
		//		thomas::object::Object* new_temp = gameObject->nativePtr;

		//		thomas::object::Object* old_native = gameObject->setStatic();

		//		GameObject^ temp = CurrentScene->Find(static_cast<thomas::object::GameObject*>(old_native));

		//		if (temp) // If temp is nullptr, no managed object has been invalidated.
		//			temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.

		//	}

		//	else if (gameObject->MakeDynamic())
		//	{
		//		thomas::object::Object* new_temp = gameObject->nativePtr;

		//		thomas::object::Object* old_native = gameObject->setDynamic();

		//		GameObject^ temp = CurrentScene->Find(static_cast<thomas::object::GameObject*>(old_native));

		//		if (temp) // If temp is nullptr, no managed object has been invalidated.
		//			temp->nativePtr = new_temp; // Nothing becomes invalidated if we don't do anything.
		//	}

		//}
#endif
	}

	void ThomasWrapper::Shutdown()
	{
		Environment::Exit(0);
	}
	
	void ThomasWrapper::CopyCommandList()
	{
		PROFILE("CopyCommandList")
		utils::profiling::GpuProfiler* profiler = utils::profiling::GpuProfiler::Instance();
		if (showStatistics)
		{
			ImGui::Begin("Statistics", &(bool&)showStatistics, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("%d FPS (%.2f ms)", ThomasTime::GetFPS(), ThomasTime::GetFrameTime());
			ImGui::Text("Logic Thread: %.02f ms	 Render Thread: %.02f ms", logicTime*1000.0f, renderTime*1000.0f);
			ImGui::Text("CPU: %.02f ms	GPU: %.02f ms", (logicTime + renderTime)*1000.0f, profiler->GetFrameTime()*1000.0f);
			ImGui::Text("Draw calls: %d	Verts: %d", profiler->GetNumberOfDrawCalls(), profiler->GetVertexCount());
			ImGui::Text("VRAM Usage: %.2f MB of (%.2f MB)", profiler->GetCurrentMemory(), 512.0f);
			ImGui::Text("RAM Usage: %.2f MB", utils::profiling::ProfileManager::getRAMUsage());
			ImGui::Text("	Draw Shadows:  %0.5f ms", profiler->GetTimeStamp(utils::profiling::GTS_SHADOWS_END)*1000.0f);
			ImGui::Text("	Main objects:  %0.5f ms", profiler->GetTimeStamp(utils::profiling::GTS_MAIN_OBJECTS_END)*1000.0f);
			ImGui::Text("	Particles:     %0.5f ms", profiler->GetTimeStamp(utils::profiling::GTS_PARTICLES_END)*1000.0f);
			ImGui::Text("	Gizmo objects: %0.5f ms", profiler->GetTimeStamp(utils::profiling::GTS_GIZMO_OBJECTS_END)*1000.0f);
			ImGui::Text("    GUI objects:   %0.5f ms", profiler->GetTimeStamp(utils::profiling::GTS_GUI_END)*1000.0f);
			ImGui::End();
		}

		if (WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->Initialized())
			WindowManager::Instance()->GetEditorWindow()->RenderGUIData();

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

	void ThomasWrapper::LoadEditorAssets()
	{
#if _EDITOR
		if (IsEditorBuild()) {
			Model::InitPrimitives();
			Resources::LoadAll(Application::editorAssets);
			ScriptingManager::Init();
		}
		else
		{
			Resources::LoadAll(Application::editorAssets + "\\FXIncludes");
		}
#endif
	}

	void ThomasWrapper::StartEngine()
	{
		LoadEditorAssets();
		// Boot up render thread
		renderThread = gcnew Thread(gcnew ThreadStart(StartRenderer));
		renderThread->Name = "Thomas Engine (Render Thread)";
		renderThread->Start();
		playing = IsEditorBuild() ? RunningState::Editor : RunningState::UnInitialized;

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
				thomas::graphics::Renderer::Instance()->ClearAllCommands();	// Clear renderer
			}
			if (Thomas->m_scene->NoSceneExist())	// Wait for scene load
			{
				Thread::Sleep(500);
				continue;
			}
			float timeStart = ThomasTime::GetElapsedTime();
			try {
				bool hasKey = thomas::WindowManager::Instance() && thomas::WindowManager::Instance()->GetGameInput()->GetLastKey() != NULL;
				thomas::ThomasTime::Update();

				if (WindowManager::Instance()->WaitingForUpdate()) //Make sure that we are not rendering when resizing the window.
					RenderFinished->WaitOne();
				WindowManager::Instance()->Update();


				ThomasCore::Update();
#ifdef _THOMAS_SCENE_LOCK
				Monitor::Enter(Thomas->m_sceneLock);
#endif _THOMAS_SCENE_LOCK


				//Logic
				{
					PROFILE("LogicLoop")
						for each (GameObject^ gameObject in CurrentScene->GameObjects)
						{
							if (gameObject->GetActive())
							{
								
								/*if (gameObject == ChadControls.gameObject)
								{
									PROFILE("Chads components");
									gameObject->Update();
								}
								else*/
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
					thomas::Physics::Simulate();
					for each (GameObject^ gameObject in CurrentScene->GameObjects)
					{
						if (gameObject->GetActive())
						{
							//PROFILE("All gameObjects")
							//if (gameObject->Name == "Chad")
							//{
							//	PROFILE("Chads components")
							//	gameObject->FixedUpdate(); //Should only be ran at fixed timeSteps.
							//}
							//else
								gameObject->FixedUpdate(); //Should only be ran at fixed timeSteps.
						}
					}
				}
#ifdef _THOMAS_SCENE_LOCK
				Monitor::Exit(Thomas->m_sceneLock);
#endif _THOMAS_SCENE_LOCK


				//Rendering
				if (WindowManager::Instance())
				{
					// Editor render
					if (WindowManager::Instance()->GetEditorWindow() && RenderEditor)
					{
						editor::EditorCamera::Instance()->Render();
						//GUI::ImguiStringUpdate(thomas::ThomasTime::GetFPS().ToString(), Vector2(Window::GetEditorWindow()->GetWidth() - 100, 0)); TEMP FPS stuff :)

						for each (GameObject^ gameObject in CurrentScene->GameObjects)
						{
							//if (gameObject->GetActive())
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
#ifdef _THOMAS_SCENE_LOCK
				Monitor::Enter(Thomas->m_sceneLock);
#endif
						RenderFinished->WaitOne();
					}
					/* Render & Update is synced.
					*/
					SynchronousExecution();
					RenderFinished->Reset();
					UpdateFinished->Set();

#ifdef _THOMAS_SCENE_LOCK
				Monitor::Exit(Thomas->m_sceneLock);
#endif

				}



				/* Run async. update on main thread.
				*/
				mainThreadDispatcher->BeginInvoke(
					System::Windows::Threading::DispatcherPriority::Normal,
					gcnew MainThreadDelegate(MainThreadUpdate));
			}
		}
		while (!renderThread->Join(System::TimeSpan(0, 0, 1)))	// Wait until thread is finished
		{
			Debug::Log("Waiting for render thread to terminate...");
		}



		WaitLogOutput = gcnew ManualResetEvent(false);
		WaitCallback^ logOut = gcnew WaitCallback(DumpProfilerLog);
		ThreadPool::QueueUserWorkItem(logOut);

		delete Thomas;				// Delete the thomas instance, unload scene...

		WaitLogOutput->WaitOne();
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

	void ThomasWrapper::IssueShutdown()
	{
		if(inEditor)
		{
			IssueStopPlay();
			return;
		}

		mainThreadDispatcher->BeginInvoke(
			System::Windows::Threading::DispatcherPriority::Normal,
			gcnew MainThreadDelegate(Shutdown));
	}

	IntPtr ThomasWrapper::CreateThomasWindow(System::String^ name, int width, int height)
	{
		HWND hwnd;
		WindowManager::Instance()->Create(hwnd, nullptr, width, height, Utility::ConvertString(name));
			
		return IntPtr(hwnd);
	}

	IntPtr ThomasWrapper::CreateThomasWindow(IntPtr parent, int width, int height, bool isEditor)
	{
		HWND hwnd;
		WindowManager::Instance()->Create(hwnd, (HWND)parent.ToPointer(), width, height, isEditor);

		if (isEditor) 
		{
			inEditor = true;
			thomas::ThomasCore::SetEditor(true);
		}

		return IntPtr(hwnd);
	}

	void ThomasWrapper::eventHandler(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam) 
{
		thomas::Window::EventHandler((HWND)hWnd.ToPointer(), msg, (WPARAM)wParam.ToPointer(), (LPARAM)lParam.ToPointer());
	}

	void ThomasWrapper::Resize(IntPtr hWnd)
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

	void ThomasWrapper::ProcessStateCommand()
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
			else if (IssuedStateCommand == ThomasStateCommand::ReplayIssued)
			{
				StopPlay();
				Play();
			}
			IssuedStateCommand = ThomasStateCommand::NoCommand;
			//StateCommandProcessed->Set();
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
		playing = RunningState::Running;			// Set running state, scene depends on this value set during initiation.
		thomas::ThomasCore::Core().OnPlay();
		ThomasEngine::Resources::OnPlay();
		// Initiate all object: Start engine, Chuuchuu!
		if (CurrentScene->OnPlay())
		{
			Debug::Log("Running...");
#ifdef _EDITOR
			OnStartPlaying();
#endif
		}
		else
		{
			playing = RunningState::Editor;
			// Error failed to run...
			StopPlay();
		}
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
			GameObject^ gObj = ThomasEngine::ThomasWrapper::CurrentScene->Find(selectedGUID);
			if (gObj)
				s_Selection->SelectGameObject(gObj);
		}

		Debug::Log("Stopped...");
		playing = RunningState::Editor;
#ifdef _EDITOR
		OnStopPlaying();
#endif
	}
	void ThomasWrapper::ENTER_SYNC_STATELOCK()
	{
		if(IsPlaying())
			Monitor::Enter(s_SYS->m_sceneLock);
	}
	void ThomasWrapper::EXIT_SYNC_STATELOCK()
	{
		if (IsPlaying() && Monitor::IsEntered(s_SYS->m_sceneLock))
			Monitor::Exit(s_SYS->m_sceneLock);
	}


	void ThomasWrapper::IssueCommand(ICommand ^ cmd)
	{
		Thomas->m_engineCommands->issue(cmd);
	}
	void ThomasWrapper::IssueStateCommand(ThomasStateCommand state)
	{
		Monitor::Enter(StateCommandProcessed);
		if(IssuedStateCommand == ThomasStateCommand::NoCommand)
			IssuedStateCommand = state;
		Monitor::Exit(StateCommandProcessed);
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
		IssuedStateCommand = ThomasStateCommand::ReplayIssued;
	}

	bool ThomasWrapper::IsPlaying()
	{
		return playing == RunningState::Running;
	}

	bool ThomasWrapper::IsEditor()
	{
		return playing == RunningState::Editor;
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

	bool ThomasWrapper::IsEditorBuild()
	{
		return inEditor;
	}

}