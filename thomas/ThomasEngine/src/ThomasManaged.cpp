// This is the main DLL file.

#pragma unmanaged

#include <thomas\ThomasCore.h>
#include <thomas\WindowManager.h>
#include <thomas\ThomasTime.h>
#include <thomas\graphics\Renderer.h>
#include <thomas\editor\gizmos\Gizmos.h>
#include <thomas\Physics.h>
#include <thomas\editor\EditorCamera.h>

#pragma managed
#include "ThomasManaged.h"
#include "resource\Model.h"
#include "resource\Resources.h"
#include "object\Component.h"
#include "object/component/physics/Rigidbody.h"
#include "ScriptingManager.h"
#include "ThomasSelection.h"
#include "GUI\editor\GUI.h"
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
			WindowManager::Instance()->ClearAllWindows();
			graphics::Renderer::Instance()->ProcessCommands();
			WindowManager::Instance()->PresentAllWindows();
			RenderFinished->Set();
		}
	}

	void ThomasWrapper::CopyCommandList()
	{
		WindowManager::Instance()->GetEditorWindow()->EndFrame(true);
		thomas::graphics::Renderer::Instance()->TransferCommandList();
		thomas::editor::Gizmos::TransferGizmoCommands();

		editor::EditorCamera::Instance()->GetCamera()->CopyFrameData();
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
			thomas::ThomasTime::Update();
			Object^ lock = Scene::CurrentScene->GetGameObjectsLock();

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
					auto collider = gameObject->GetComponent<Rigidbody^>()->GetTargetCollider();
					if (collider != nullptr)
					{
						gameObject->OnCollisionEnter(collider);
					}

					gameObject->Update();
				}
			}

			//Rendering
			thomas::graphics::Renderer::Instance()->ClearCommands();
			editor::Gizmos::ClearGizmos();
			if (WindowManager::Instance()->GetEditorWindow() && WindowManager::Instance()->GetEditorWindow()->Initialized())
			{
				if (renderingEditor)
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
		Resources::UnloadAll();
		ThomasCore::Destroy();

	}

	void ThomasWrapper::Exit() {
		thomas::ThomasCore::Exit();
	}

	void ThomasWrapper::CreateThomasWindow(IntPtr hWnd, bool isEditor)
	{
		if (thomas::ThomasCore::Initialized())
			WindowManager::Instance()->Create((HWND)hWnd.ToPointer(), isEditor);
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
		UpdateLog();
		if (editor::EditorCamera::Instance()->HasSelectionChanged())
			s_Selection->UpdateSelectedObjects();
	}

	Guid selectedGUID;
	void ThomasWrapper::Play()
	{
		ThomasEngine::Resources::OnPlay();
		Scene::CurrentScene->Play();
		playing = true;

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

	void ThomasWrapper::UpdateLog() 
	{
		std::vector<std::string> nativeOutputs = thomas::ThomasCore::GetLogOutput();

		for (int i = 0; i < nativeOutputs.size(); i++) {
			String^ output = gcnew String(nativeOutputs.at(i).c_str());
			if (OutputLog->Count == 0 || OutputLog[OutputLog->Count - 1] != output)
			{
				OutputLog->Add(output);
				if (OutputLog->Count > 10)
					OutputLog->RemoveAt(0);
			}
		}
		thomas::ThomasCore::ClearLogOutput();
	}
	void ThomasWrapper::ToggleEditorRendering()
	{
		renderingEditor = !renderingEditor;
	}
}