// This is the main DLL file.

#pragma unmanaged

#include <thomas\ThomasCore.h>
#include <thomas\Window.h>
#include <thomas\ThomasTime.h>
#include <thomas\graphics\Renderer.h>
#include <thomas\editor\gizmos\Gizmos.h>
#include <thomas\Physics.h>
#include <thomas\editor\EditorCamera.h>
#include <thomas\System.h>
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
			Window::ClearAllWindows();
			thomas::System::S_RENDERER.ProcessCommands();
			thomas::Window::PresentAllWindows();
			RenderFinished->Set();
		}
	}

	void ThomasWrapper::CopyCommandList()
	{
		thomas::Window::EndFrame(true);
		thomas::System::S_RENDERER.TransferCommandList();
		thomas::editor::Gizmos::TransferGizmoCommands();

		editor::EditorCamera::GetEditorCamera()->GetCamera()->CopyFrameData();
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
			Object^ lock = Scene::CurrentScene->GetGameObjectsLock();
			try {

				thomas::ThomasTime::Update();
				

				if (Window::WaitingForUpdate()) //Make sure that we are not rendering when resizing the window.
					RenderFinished->WaitOne();
				Window::Update();


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
						auto collider = gameObject->GetComponent<Rigidbody^>()->GetTargetCollider();
						if (collider != nullptr)
						{
							gameObject->OnCollisionEnter(collider);
						}

						gameObject->Update();
					}
				}

				//Rendering

				thomas::System::S_RENDERER.ClearCommands();
				editor::Gizmos::ClearGizmos();
				if (Window::GetEditorWindow() && Window::GetEditorWindow()->Initialized())
				{
					if (renderingEditor)
					{
						editor::EditorCamera::Render();
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
				}

				
			}
			catch (Exception^ e) {
				Debug::LogException(e);
				if (playing) {
					for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++)
					{
						GameObject^ g = Scene::CurrentScene->GameObjects[i];
						Monitor::Exit(g->m_componentsLock);
					}
					Stop();
				}
					
					
			}finally
			{
				if (Window::GetEditorWindow() && Window::GetEditorWindow()->Initialized())
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
		thomas::ThomasCore::Exit();
	}

	void ThomasWrapper::CreateThomasWindow(IntPtr hWnd, bool isEditor)
	{
		if (thomas::ThomasCore::Initialized()) {
			if (isEditor)
				thomas::Window::InitEditor((HWND)hWnd.ToPointer());
			else
				thomas::Window::Create((HWND)hWnd.ToPointer());

		}

	}


	void ThomasWrapper::eventHandler(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam) {
		thomas::Window::EventHandler((HWND)hWnd.ToPointer(), msg, (WPARAM)wParam.ToPointer(), (LPARAM)lParam.ToPointer());
	}

	void ThomasWrapper::Resize(IntPtr hWnd, double width, double height)
	{
		Window* window = thomas::Window::GetWindow((HWND)hWnd.ToPointer());
		if (window)
			window->QueueResize();
	}

	void ThomasWrapper::Update()
	{
		Window::UpdateFocus();
		Debug::UpdateCoreLog();
		if (thomas::editor::EditorCamera::GetEditorCamera()->HasSelectionChanged())
			s_Selection->UpdateSelectedObjects();
	}

	Guid selectedGUID;
	void ThomasWrapper::Play()
	{
		ThomasEngine::Resources::OnPlay();
		Scene::CurrentScene->Play();
		playing = true;
		OnStartPlaying();

	}

	bool ThomasWrapper::IsPlaying()
	{
		return playing;
	}

	void ThomasWrapper::Stop()
	{
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

	}

	float ThomasWrapper::FrameRate::get() { return float(thomas::ThomasTime::GetFPS()); }

	void ThomasWrapper::SetEditorGizmoManipulatorOperation(ManipulatorOperation op)
	{
		thomas::editor::EditorCamera::GetEditorCamera()->SetManipulatorOperation((ImGuizmo::OPERATION)op);
	}

	ThomasWrapper::ManipulatorOperation ThomasWrapper::GetEditorGizmoManipulatorOperation()
	{
		return (ManipulatorOperation)thomas::editor::EditorCamera::GetManipulatorOperation();
	}

	void ThomasWrapper::ToggleEditorGizmoManipulatorMode()
	{
		thomas::editor::EditorCamera::GetEditorCamera()->ToggleManipulatorMode();
	}

	
	void ThomasWrapper::ToggleEditorRendering()
	{
		renderingEditor = !renderingEditor;
	}
}