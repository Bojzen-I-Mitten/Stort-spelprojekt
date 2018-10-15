
#pragma unmanaged
#include <thomas/editor/EditorCamera.h>

#pragma managed
#include "Utility.h"
#include "Scene.h"
#include "ThomasSelection.h"
#include "object\GameObject.h"
#include "object\Component.h"
#include "resource\Resources.h"
#include "Debug.h"
#include "Application.h"
#include "Project.h"
#include "serialization/Serializer.h"

namespace ThomasEngine
{
	Scene::Scene(System::String^ name) 
	:	m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = name;;
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}
	Scene::Scene()
		: m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = "New Scene";
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}

	void Scene::Play()
	{
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		savingEnabled = false;
		SaveSceneAs(this, tempFile);
		m_playing = true;
	}

	Scene^ Scene::CurrentScene::get()
	{
		return s_currentScene;
	}

	void Scene::CurrentScene::set(Scene^ value)
	{
		Scene^ oldScene = s_currentScene;
		if(value == nullptr)	// If clear scene
			value = gcnew Scene("test");
		ThomasWrapper::Selection->UnselectGameObjects();
		s_currentScene = value;
		if(Application::currentProject && savingEnabled)
			Application::currentProject->currentScenePath = value->m_relativeSavePath;

		OnCurrentSceneChanged(oldScene, value);
	}


	void Scene::SaveSceneAs(Scene ^ scene, System::String ^ path)
	{
		Serializer::SerializeScene(scene, path);

		if (Application::currentProject && scene->RelativeSavePath != path && savingEnabled) {
			scene->m_relativeSavePath = path->Replace(Application::currentProject->assetPath + "\\", "");
			Application::currentProject->currentScenePath = scene->RelativeSavePath;
		}
			
	}

	void Scene::SaveScene(Scene ^ scene)
	{
		if(Application::currentProject && scene->RelativeSavePath)
			SaveSceneAs(scene, Application::currentProject->assetPath + "\\" + scene->RelativeSavePath);
	}

	Scene ^ Scene::LoadScene(System::String ^ fullPath)
	{
		if (!IO::File::Exists(fullPath))
		{
			Debug::LogError("Unable to find scene: " + fullPath);
			return nullptr;
		}
		Scene^ scene;
		try
		{
			s_loading = true;
			scene = Serializer::DeserializeScene(fullPath);
			scene->EnsureLoad();

			for (int i = 0; i < scene->GameObjects->Count; ++i)
				scene->GameObjects[i]->nativePtr->SetName(Utility::ConvertString(scene->GameObjects[i]->Name));

			scene->PostLoad();
			if (Application::currentProject && savingEnabled)
				scene->m_relativeSavePath = fullPath->Replace(Application::currentProject->assetPath + "\\", "");
		}
		catch (Exception^ e) {
			Debug::LogError("Failed loading scene: " + fullPath + "\nError: " + e->Message);
			scene = nullptr;
		}
		finally{
			s_loading = false;
		}
		return scene;
	}

	void Scene::RestartCurrentScene()
	{
		Object^ lock = Scene::CurrentScene->GetGameObjectsLock();
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		Monitor::Enter(lock);
		Scene::CurrentScene->UnLoad();
		Scene::CurrentScene = Scene::LoadScene(tempFile);
		try {
			System::IO::File::Delete(tempFile);
		}
		catch (Exception^ e)
		{
			Debug::LogError("Error in ThomasEngine::Scene. Msg: " + e->Message);
		}
		Monitor::Exit(lock);
		savingEnabled = true;
	}

	void Scene::UnLoad()
	{
		for (int i = 0; i < m_gameObjects->Count; i++)
		{
			m_gameObjects[i]->Destroy();
			i--;
		}
		m_gameObjects->Clear();

	}

	void Scene::EnsureLoad()
	{

		if (m_gameObjects == nullptr) { // Scene is empty
			Debug::LogWarning("Warning, no objects in scene.");
			m_gameObjectsLock = gcnew Object();
			m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
			System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
		}

	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}
	}





	Vector3 Scene::CameraPosition::get() 
	{
		return Utility::Convert(thomas::editor::EditorCamera::Instance()->m_transform->GetLocalPosition());
	}

	void Scene::CameraPosition::set(Vector3 pos) 
	{

		thomas::editor::EditorCamera::Instance()->m_transform->SetLocalPosition(Utility::Convert(pos));
	}

	Vector3 Scene::CameraEuler::get() 
	{
		return Utility::Convert(thomas::editor::EditorCamera::Instance()->m_transform->GetLocalEulerAngles());
	}

	void Scene::CameraEuler::set(Vector3 euler) 
	{

		thomas::editor::EditorCamera::Instance()->m_transform->SetLocalRotation(euler.y, euler.x, euler.z);
	}

}

