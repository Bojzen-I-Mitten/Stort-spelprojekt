
#pragma unmanaged
#include <thomas/editor/EditorCamera.h>
#include <thomas/object/ObjectHandler.h>

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
#include "ThomasSelection.h"
#include "system/SceneManager.h"

namespace ThomasEngine
{
	Scene::Scene(System::String^ name, uint32_t unique_id)
	:	m_uniqueID(unique_id),
		m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = name;;
		//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}
	Scene::Scene(uint32_t unique_id)
	:	m_uniqueID(unique_id),
		m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = "New Scene";
		//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}
	Scene::~Scene()
	{
		UnLoad();
	}


	void Scene::InitGameObjects(bool playing) {
		bool completed;
		do {
			completed = true;
			for (int i = 0; i < m_gameObjects->Count; ++i) {
				GameObject^ gameObject = m_gameObjects[i];
				if (gameObject->GetActive())
					completed = gameObject->InitComponents(playing) && completed;
			}
		} while (!completed);
	}

	void Scene::OnPlay()
	{
		ThomasWrapper::Thomas->SceneManagerRef->StoreTempCopy();
		// Initiate 
		InitGameObjects(true);
	}


	void Scene::CreateObject(GameObject ^ object)
	{
		throw gcnew System::NotImplementedException();
	}

	void Scene::DestroyObject(GameObject ^ object)
	{

		ThomasWrapper::Selection->UnSelectGameObject(object);

		// Remove object
		Monitor::Enter(m_accessLock);
		m_gameObjects->Remove(object);
		Monitor::Exit(m_accessLock);
		// Destroy object
		object->OnDestroy();
		delete object;
	}


	void Scene::UnLoad()
	{
		Monitor::Enter(m_accessLock);
		for (int i = 0; i < m_gameObjects->Count; i++)
		{
			m_gameObjects[i]->OnDestroy();
			delete m_gameObjects[i];
		}
		m_gameObjects->Clear();
		Monitor::Exit(m_accessLock);
	}

#pragma region Save scene

	void Scene::SaveSceneAs(System::String ^ path)
	{
		Serializer::SerializeScene(this, path);

		if (Application::currentProject && this->RelativeSavePath != path) 
			this->m_relativeSavePath = path->Replace(Application::currentProject->assetPath + "\\", "");
			m_gameObjects->CollectionChanged
	}

	void Scene::SaveScene()
	{
		if(Application::currentProject && this->RelativeSavePath)
			SaveSceneAs(Application::currentProject->assetPath + "\\" + m_relativeSavePath);
	}

	void Scene::Add(GameObject ^ obj)
	{
		IssuedCommand cmd;
		cmd.m_cmd = Command::Add;
		cmd.m_obj = obj;
		m_commandList->Add(cmd);
	}

	void Scene::Remove(GameObject ^ obj)
	{
		IssuedCommand cmd;
		cmd.m_cmd = Command::Remove;
		cmd.m_obj = obj;
		m_commandList->Add(cmd);
	}


	Scene ^ Scene::LoadScene(System::String ^ fullPath, uint32_t unique_id)
	{
		if (!IO::File::Exists(fullPath))
		{
			Debug::LogError("Unable to find scene: " + fullPath);
			return nullptr;
		}
		Scene^ scene;
		try
		{
			scene = Serializer::DeserializeScene(fullPath);
			scene->EnsureLoad();

			for (int i = 0; i < scene->m_gameObjects->Count; ++i)
				scene->m_gameObjects[i]->nativePtr->SetName(Utility::ConvertString(scene->m_gameObjects[i]->Name));

			//if (Application::currentProject)
			//	scene->m_relativeSavePath = fullPath->Replace(Application::currentProject->assetPath + "\\", "");
			scene->PostLoad();            
			scene->m_uniqueID = unique_id;
		}
		catch (Exception^ e) {
			Debug::LogError("Failed loading scene: " + fullPath + "\nError: " + e->Message);
			scene = nullptr;
		}
		finally{
			// Loading complete
		}
		return scene;
	}


	void Scene::EnsureLoad()
	{

		if (m_gameObjects == nullptr) { // Scene is empty
			Debug::LogWarning("Warning, no objects in scene.");
			m_accessLock = gcnew Object();
			m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
			//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
		}

	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}
	}

#pragma endregion

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

	System::Collections::Generic::IEnumerable<GameObject^>^ Scene::GameObjects::get()
	{
		return m_gameObjects;
	}
	System::Collections::ObjectModel::ObservableCollection<GameObject^>^ Scene::GameObjectData::get() {
		return m_gameObjects;
	}
	void Scene::GameObjectData::set(System::Collections::ObjectModel::ObservableCollection<GameObject^>^ val) {
		m_gameObjects = val;
		//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}
}

