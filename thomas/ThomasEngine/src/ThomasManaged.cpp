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

		String^ enginePath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location);

		Environment::SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget::User);

		s_Selection = gcnew ThomasSelection();
		Thread::CurrentThread->Name = "Main Thread";
		thomas::ThomasCore::Init();
		if (ThomasCore::Initialized())
		{
			Model::InitPrimitives();
			Resources::LoadAll(Application::editorAssets);
			Component::LoadExternalComponents();


			RenderFinished = gcnew ManualResetEvent(true);
			UpdateFinished = gcnew ManualResetEvent(false);
			ScriptingManger::Init();
			Scene::CurrentScene = gcnew Scene("test");
			LOG("Thomas fully initiated, Chugga-chugga-whoo-whoo!");
			mainThread = gcnew Thread(gcnew ThreadStart(StartEngine));
			mainThread->Name = "Thomas Engine (Main Thread)";
			mainThread->Start();

			renderThread = gcnew Thread(gcnew ThreadStart(StartRenderer));
			renderThread->Name = "Thomas Engine (Render Thread)";
			renderThread->Start();
		}
	}


	void ThomasWrapper::StartRenderer()
	{

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

		float ramUsage = float(System::Diagnostics::Process::GetCurrentProcess()->PrivateMemorySize64 / 1024.0f / 1024.0f);
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
			ImGui::Text("	Gizmo objects: %0.2f ms", profiler->GetAverageTiming(profiling::GTS_GIZMO_OBJECTS)*1000.0f);
			ImGui::End();
		}

		if(WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->Initialized())
			WindowManager::Instance()->GetEditorWindow()->EndFrame(true);
		thomas::graphics::Renderer::Instance()->TransferCommandList();
		thomas::editor::Gizmos::TransferGizmoCommands();

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
		while (ThomasCore::Initialized())
		{
			if (Scene::IsLoading() || Scene::CurrentScene == nullptr)
			{
				Thread::Sleep(1000);
				continue;
			}
			NEW_FRAME();
			float timeStart = ThomasTime::GetElapsedTime();
			PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic);
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

				//Rendering
				thomas::graphics::Renderer::Instance()->ClearCommands();
				editor::Gizmos::ClearGizmos();
				if (WindowManager::Instance())
				{
					if (WindowManager::Instance()->GetEditorWindow() && renderingEditor)
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
					if (WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->GetInput()->GetKeyDown(Input::Keys::F1)) {
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
					Stop();
				}				
			}
			finally
			{
				cpuTime = ThomasTime::GetElapsedTime() - timeStart;
				if (WindowManager::Instance())
				{
					thomas::object::component::RenderComponent::ClearList();
					RenderFinished->WaitOne();
					thomas::graphics::LightManager::Update();
					CopyCommandList();
					RenderFinished->Reset();
					UpdateFinished->Set();
				}
				Monitor::Exit(lock);
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

	Guid selectedGUID;
	void ThomasWrapper::Play()
	{
#ifdef _EDITOR
		thomas::editor::Editor::GetEditor().OnEditorPlay();
#endif
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

	void ThomasWrapper::Stop()
	{
#ifdef _EDITOR
		thomas::editor::Editor::GetEditor().OnEditorStop();
#endif
		if (s_Selection->Count > 0)
			selectedGUID = s_Selection[0]->m_guid;
		else
			selectedGUID = Guid::Empty;
		playing = false;
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