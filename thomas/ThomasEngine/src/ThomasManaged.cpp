// This is the main DLL file.

#pragma unmanaged

#include <thomas\ThomasCore.h>
#include <thomas\WindowManager.h>
#include <thomas\ThomasTime.h>
#include <thomas\graphics\Renderer.h>
#include <thomas\editor\gizmos\Gizmos.h>
#include <thomas\Physics.h>
#include <thomas\editor\Editor.h>
#include <thomas\editor\EditorCamera.h>
#include <thomas\AutoProfile.h>
#include <thomas\ProfileManager.h>
#include <thomas\utils\GpuProfiler.h>
#include <thomas\object\component\Camera.h>
#include <thomas\object\component\RenderComponent.h>
#include <thomas\graphics\LightManager.h>
#pragma managed
#include "ThomasManaged.h"
#include "resource\Model.h"
#include "resource\Resources.h"
#include "object\Component.h"
#include "object/component/physics/Rigidbody.h"
#include "ScriptingManager.h"
#include "ThomasSelection.h"
#include "GUI\editor\GUI.h"
#include "object/GameObject.h"
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
		// System initialization
		Thread::CurrentThread->Name = "Main Thread";
		String^ enginePath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);
		Environment::SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget::User);

		// Thomas Initialization
		Thomas->m_scene = gcnew SceneManager();
		s_Selection = gcnew ThomasSelection();
		thomas::ThomasCore::Core().registerThread();
		thomas::ThomasCore::Init();



		if (ThomasCore::Initialized())
		{
			Model::InitPrimitives();
			Resources::LoadAll(Application::editorAssets);
			Component::LoadExternalComponents();

			RenderFinished = gcnew ManualResetEvent(true);
			UpdateFinished = gcnew ManualResetEvent(false);
			StateCommandProcessed = gcnew ManualResetEvent(false);
			ScriptingManger::Init();
			Thomas->m_scene->LogicThreadClearScene();
			LOG("Thomas fully initiated, Chugga-chugga-whoo-whoo!");
			mainThread = gcnew Thread(gcnew ThreadStart(StartEngine));
			mainThread->Name = "Thomas Engine (Logic Thread)";
			mainThread->Start();

			renderThread = gcnew Thread(gcnew ThreadStart(StartRenderer));
			renderThread->Name = "Thomas Engine (Render Thread)";
			renderThread->Start();
		}
	}


	void ThomasWrapper::StartRenderer()
	{
		// Render thread start
		ThomasCore::Core().registerThread();
		while (ThomasCore::Initialized())
		{
			UpdateFinished->WaitOne();
			UpdateFinished->Reset();
			ThomasCore::Render();
			RenderFinished->Set();
		}
	}
	
	void ThomasWrapper::CopyCommandList()
	{

		float ramUsage = 0;//float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 / 1024.0f / 1024.0f);
		profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
		profiler->SetActive(showStatistics);
		if (showStatistics) {
			ImGui::Begin("Statistics", &(bool&)showStatistics, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("%d FPS (%.2f ms)", ThomasTime::GetFPS(), ThomasTime::GetFrameTime());
			ImGui::Text("Main thread: %.02f ms	Render thread: %.02f ms", cpuTime*1000.0f, profiler->GetFrameTime()*1000.0f);
			ImGui::Text("Draw calls: %d	Verts: %d", profiler->GetNumberOfDrawCalls(), profiler->GetVertexCount());
			ImGui::Text("VRAM Usage: %.2f MB (of %.2f MB)", profiler->GetMemoryUsage(), profiler->GetTotalMemory());
			ImGui::Text("RAM Usage: %.2f MB", ramUsage);
			ImGui::Text("Draw time: %0.2f ms", profiler->GetDrawTotal()*1000.0f);
			ImGui::Text("	Window clear: %0.2f ms", profiler->GetAverageTiming(profiling::GTS_MAIN_CLEAR)*1000.0f);
			ImGui::Text("	Main objects: %0.2f ms", profiler->GetAverageTiming(profiling::GTS_MAIN_OBJECTS)*1000.0f);
			ImGui::Text("	Particles: %0.2f ms", profiler->GetAverageTiming(profiling::GTS_PARTICLES)*1000.0f);
			ImGui::Text("	Gizmo objects: %0.2f ms", profiler->GetAverageTiming(profiling::GTS_GIZMO_OBJECTS)*1000.0f);
			ImGui::End();
		}

		if(WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->Initialized())
			WindowManager::Instance()->GetEditorWindow()->EndFrame(true);

		// Swap command lists
		thomas::graphics::Renderer::Instance()->TransferCommandList();
		thomas::editor::Gizmos::Gizmo().TransferGizmoCommands();
		thomas::graphics::Renderer::Instance()->ClearCommands();
		thomas::editor::Gizmos::Gizmo().ClearGizmos();

		editor::EditorCamera::Instance()->GetCamera()->CopyFrameData();
//#ifdef _EDITOR
//		editor::Editor::GetEditor().Camera()->GetCamera()->CopyFrameData();
//#endif
		for (object::component::Camera* camera : object::component::Camera::s_allCameras)
		{
			camera->CopyFrameData();
		}
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
			NEW_FRAME();
			float timeStart = ThomasTime::GetElapsedTime();
			PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic);
			Object^ lock = CurrentScene->GetGameObjectsLock();
			try {

				thomas::ThomasTime::Update();
				
				if (WindowManager::Instance()->WaitingForUpdate()) //Make sure that we are not rendering when resizing the window.
					RenderFinished->WaitOne();
				WindowManager::Instance()->Update();


				ThomasCore::Update();
				Monitor::Enter(lock);

				CurrentScene->InitGameObjects(IsPlaying());

				if (IsPlaying())
				{
					//Physics
					thomas::Physics::UpdateRigidbodies();
					for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ gameObject = CurrentScene->GameObjects[i];
						if (gameObject->GetActive())
							gameObject->FixedUpdate(); //Should only be ran at fixed timeSteps.
					}
					thomas::Physics::Simulate();
				}

				//Logic
				for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
				{
					GameObject^ gameObject = CurrentScene->GameObjects[i];
					if (gameObject->GetActive())
					{
						gameObject->Update();
					}
				}
				editor::EditorCamera::Instance()->Update();

				//Rendering
				if (WindowManager::Instance())
				{
					// Editor render
					if (WindowManager::Instance()->GetEditorWindow() && renderingEditor)
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
				
					

					for (object::component::Camera* camera : object::component::Camera::s_allCameras)
					{
						camera->Render();
					}
					if (WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->GetInput()->GetKeyDown(Keys::F1)) {
						showStatistics = !showStatistics;
					}
				}
			}
			catch (Exception^ e) {
				Debug::LogException(e);
				if (playing) {
					for (int i = 0; i < CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ g = CurrentScene->GameObjects[i];
						if(Monitor::IsEntered(g->m_componentsLock))
							Monitor::Exit(g->m_componentsLock);
					}
				}	
			}
			finally
			{
				cpuTime = ThomasTime::GetElapsedTime() - timeStart;
				if (WindowManager::Instance())
				{
					thomas::object::component::RenderComponent::ClearList();

					// Wait for renderer
					RenderFinished->WaitOne();
					
					/* Render & Update is synced.
					*/
					thomas::graphics::LightManager::Update();
					CopyCommandList();

					// Process state switch commands
					ProcessCommand();
					
					// Enter async. state 
					RenderFinished->Reset();
					UpdateFinished->Set();
				}
				Monitor::Exit(lock);
				if(!playing)
					ScriptingManger::ReloadIfNeeded();
			}
		}
		Resources::UnloadAll();
		ThomasCore::Destroy();

	}

	void ThomasWrapper::Exit() {
		ProfileManager::dumpDataToFile("data.csv");
	
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
		
		playing = RunningState::Loading;
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

	bool ThomasWrapper::IsPlaying()
	{
		return playing == RunningState::Running;
	}

	bool ThomasWrapper::IsLoading()
	{
		return playing == RunningState::Loading;
	}

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

	void ThomasWrapper::ToggleEditorRendering()
	{
		renderingEditor = !renderingEditor;
	}
	void ThomasWrapper::TogglePhysicsDebug()
	{
		Physics::s_drawDebug = !Physics::s_drawDebug;
	}
}