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
#include "object/component/physics/Rigidbody.h"
#include "ScriptingManager.h"
#include "ThomasSelection.h"
#include "GUI\editor\GUI.h"
#include "object/GameObject.h"
#include "Debug.h"

using namespace thomas;

namespace ThomasEngine {

	void ThomasWrapper::Start() {
		Start(true);
	}
	void ThomasWrapper::Start(bool editor) {

		String^ enginePath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);

		Environment::SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget::User);

		s_Selection = gcnew ThomasSelection();
		Thread::CurrentThread->Name = "Main Thread";
		thomas::ThomasCore::Core().registerThread();
		thomas::ThomasCore::Init();
		if (ThomasCore::Initialized())
		{
			Model::InitPrimitives();
			Resources::LoadAll(Application::editorAssets);
			Component::LoadExternalComponents();

			RenderFinished = gcnew ManualResetEvent(true);
			UpdateFinished = gcnew ManualResetEvent(false);
			if (editor) {
				ScriptingManger::Init();
				Scene::CurrentScene = gcnew Scene("test");
			}

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
			PROFILE("StartRenderer %f")
			{
				PROFILE("StartRenderer - Wait %f")
				UpdateFinished->WaitOne();
			}
			UpdateFinished->Reset();
			ThomasCore::Render();
			RenderFinished->Set();
		}
	}

	void ThomasWrapper::CopyCommandList()
	{

		float ramUsage = 0;//float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 / 1024.0f / 1024.0f);
		utils::profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();

		profiler->SetActive(showStatistics);
		if (showStatistics) {

			ImGui::Begin("Statistics", &(bool&)showStatistics, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("%d FPS (%.2f ms)", ThomasTime::GetFPS(), ThomasTime::GetFrameTime());
			ImGui::Text("Main thread: %.02f ms	Render thread: %.02f ms", cpuTime*1000.0f, profiler->GetFrameTime()*1000.0f);
			ImGui::Text("Draw calls: %d	Verts: %d", profiler->GetNumberOfDrawCalls(), profiler->GetVertexCount());
			ImGui::Text("VRAM Usage: %.2f MB (of %.2f MB)", profiler->GetMemoryUsage(), profiler->GetTotalMemory());
			ImGui::Text("RAM Usage: %.2f MB", ramUsage);
			ImGui::Text("Draw time: %0.2f ms", profiler->GetDrawTotal()*1000.0f);
			ImGui::Text("	Window clear: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_MAIN_CLEAR)*1000.0f);
			ImGui::Text("	Main objects: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_MAIN_OBJECTS)*1000.0f);
			ImGui::Text("	Particles: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_PARTICLES)*1000.0f);
			ImGui::Text("	Gizmo objects: %0.2f ms", profiler->GetAverageTiming(utils::profiling::GTS_GIZMO_OBJECTS)*1000.0f);
			//for (auto& it : *utils::profiling::ProfileManager::GetData())
			//{
			//	//ImGui::PlotHistogram(it.first, it.second.data(), it.second.size(), 0, "", 0.0f, 10000.0f, ImVec2(0, 80));
			//	ImGui::Text(it.first, it.second.back());
			//}
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

	void ThomasWrapper::StartEngine()
	{
		// Update thread start

		ThomasCore::Core().registerThread();
		while (ThomasCore::Initialized())
		{
			PROFILE("StartEngine")
			if (Scene::IsLoading() || Scene::CurrentScene == nullptr)
			{
				Thread::Sleep(1000);
				continue;
			}
			float timeStart = ThomasTime::GetElapsedTime();

			Object^ lock = Scene::CurrentScene->GetGameObjectsLock();
			try {

				thomas::ThomasTime::Update();
				
				if (WindowManager::Instance()->WaitingForUpdate()) //Make sure that we are not rendering when resizing the window.
					RenderFinished->WaitOne();
				WindowManager::Instance()->Update();


				ThomasCore::Update();
				Monitor::Enter(lock);

				GameObject::InitGameObjects(playing);
				if (playing)
				{
					//Physics
					thomas::Physics::UpdateRigidbodies();
					for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ gameObject = Scene::CurrentScene->GameObjects[i];
						if (gameObject->GetActive())
							gameObject->FixedUpdate(); //Should only be ran at fixed timeSteps.
					}
					thomas::Physics::Simulate();
				}

				//Logic
				for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++)
				{
					GameObject^ gameObject = Scene::CurrentScene->GameObjects[i];
					if (gameObject->GetActive())
					{
						gameObject->Update();
					}
				}
				editor::EditorCamera::Instance()->Update();

				//Rendering
				if (WindowManager::Instance())
				{
					if (WindowManager::Instance()->GetEditorWindow() && RenderEditor)
					{
						editor::EditorCamera::Instance()->Render();
						//GUI::ImguiStringUpdate(thomas::ThomasTime::GetFPS().ToString(), Vector2(Window::GetEditorWindow()->GetWidth() - 100, 0)); TEMP FPS stuff :)
						for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++)
						{
							GameObject^ gameObject = Scene::CurrentScene->GameObjects[i];
							if (gameObject->GetActive())
								gameObject->RenderGizmos();
						}

							s_Selection->render();
					}
				
					//end editor rendering

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
					for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ g = Scene::CurrentScene->GameObjects[i];
						if(Monitor::IsEntered(g->m_componentsLock))
							Monitor::Exit(g->m_componentsLock);
					}
					IssueStop();
				}	
			}
			finally
			{
				cpuTime = ThomasTime::GetElapsedTime() - timeStart;
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
					thomas::graphics::LightManager::Update();


					CopyCommandList();
					if (shouldStop)
						Stop();
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
		float ramUsage = float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 / 1024.0f / 1024.0f);
		utils::profiling::ProfileManager::SetRAMUsage(ramUsage);
		utils::profiling::ProfileManager::dumpDataToFile();
	
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

	Guid selectedGUID;
	void ThomasWrapper::Play()
	{
		thomas::ThomasCore::Core().OnPlay();
		ThomasEngine::Resources::OnPlay();
		Scene::CurrentScene->Play();
		playing = true;
		OnStartPlaying();

		Debug::Log("Running...");
	}

	bool ThomasWrapper::IsPlaying()
	{
		return playing;
	}

	void ThomasWrapper::IssueStop()
	{
		shouldStop = true;
	}
	
	void ThomasWrapper::Stop()
	{
		playing = false;
		// Synced state
		thomas::ThomasCore::Core().OnStop();

		if (s_Selection->Count > 0)
			selectedGUID = s_Selection[0]->m_guid;
		else
			selectedGUID = Guid::Empty;
		Scene::RestartCurrentScene();
		ThomasEngine::Resources::OnStop();
		if (selectedGUID != Guid::Empty)
		{
			GameObject^ gObj = (GameObject^)ThomasEngine::Object::Find(selectedGUID);
			if (gObj)
				s_Selection->SelectGameObject(gObj);
		}
		OnStopPlaying();

		Debug::Log("Stopped...");
		shouldStop = false;
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