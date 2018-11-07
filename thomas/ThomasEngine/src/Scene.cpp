
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
		m_gameObjects(gcnew List<GameObject^>()),
		m_accessLock(gcnew System::Object()),
		m_commandList(gcnew List<IssuedCommand>()),
		m_commandSwapList(gcnew List<IssuedCommand>()),
		m_name(name)
	{
	}
	Scene::Scene(uint32_t unique_id)
		: Scene("New Scene", unique_id)
	{	}
	Scene::~Scene()
	{
		UnLoad();
	}

	void startComponents(List<GameObject^>^ objects)
	{
		if (objects->Count == 0) return;
		// Verify non-editor components are activated
		for each (GameObject^ g in objects)
			g->InitComponents(Comp::State::Enabled, true);
		// Start components
		for each (GameObject^ g in objects)
			g->StartComponents();
	}

	/* Initial initiate loop over a set of game objects
	*/
	void initiateGameObjects(List<GameObject^>^ objects)
	{
		// INn
		for each (GameObject^ g in objects)
			g->InitComponents(Comp::State::Awake, false);
#ifdef _EDITOR
		// Verify non-editor components are activated
		for each (GameObject^ g in objects)
			g->InitComponents(Comp::State::Enabled, false);
#endif
	}

	bool Scene::OnPlay()
	{
		ThomasWrapper::Thomas->SceneManagerRef->StoreTempCopy();
		try
		{
			// Start game objects
			startComponents(m_gameObjects);
		}
		catch (Exception^ e)
		{
			// Inverted order..!
			Debug::LogException(e);
			Debug::LogError("Failed to initiate game objects with following exception:");
			return false;
		}
		return true;
	}


	void Scene::CreateObject(GameObject ^ object)
	{
		IssuedCommand cmd;
		cmd.m_cmd = Command::Add;
		cmd.m_obj = object;
		m_commandList->Add(cmd);
	}

	void Scene::DestroyObject(GameObject ^ object)
	{
		IssuedCommand cmd;
		cmd.m_cmd = Command::DisableRemove;
		cmd.m_obj = object;
		m_commandList->Add(cmd);
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
		m_commandList->Clear();
		m_commandSwapList->Clear();
		Monitor::Exit(m_accessLock);
	}

#pragma region Save scene

	void Scene::SaveSceneAs(System::String ^ path)
	{
		Serializer::SerializeScene(this, path);

		if (Application::currentProject && this->RelativeSavePath != path) 
			this->m_relativeSavePath = path->Replace(Application::currentProject->assetPath + "\\", "");
	}

	void Scene::SaveScene()
	{
		if(Application::currentProject && this->RelativeSavePath)
			SaveSceneAs(Application::currentProject->assetPath + "\\" + m_relativeSavePath);
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
			m_gameObjects = gcnew List<GameObject^>();
			//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
		}

	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}

		initiateGameObjects(m_gameObjects);
	}


#pragma endregion

	void Scene::SyncScene()
	{
#ifdef _EDITOR
		Monitor::Enter(m_gameObjects);
		uint32_t numChanged = 0;
		List<GameObject^>^ addedList = gcnew List<GameObject^>();
		List<GameObject^>^ removedList = gcnew List<GameObject^>();
#endif

		for each(IssuedCommand c in m_commandList)
		{
			switch (c.m_cmd)
			{
			case Command::Add:
				m_gameObjects->Add(c.m_obj);
#ifdef _EDITOR
				numChanged++;
				addedList->Add(c.m_obj);
#endif
				break;
			case Command::DisableRemove:
			{
				// Disable object:
				ThomasWrapper::Selection->UnSelectGameObject(c.m_obj);
				m_gameObjects->Remove(c.m_obj);
				c.m_obj->OnDestroy();
#ifdef _EDITOR
				numChanged++;
				removedList->Add(c.m_obj);
#endif
				// Wait to next frame for delete:
				IssuedCommand cmd;
				cmd.m_cmd = Command::Remove;
				cmd.m_obj = c.m_obj;
				m_commandSwapList->Add(cmd);
			}
				break;
			case Command::Remove:
				// Destroy object
				delete c.m_obj;
				break;
			default:
				Debug::LogWarning("Scene: Unknown Command issued...");
			}
		}
		m_commandList->Clear();
		// Swap, std::swap plz give ._,
		auto swp = m_commandList;
		m_commandList = m_commandSwapList;
		m_commandSwapList = swp;
		// Initiate related components
		initiateGameObjects(addedList);
		if(ThomasWrapper::IsPlaying())
			startComponents(addedList);

#ifdef _EDITOR
		if(numChanged)	// Trigger event 
			m_changeEvent(this, gcnew SceneObjectsChangedArgs(numChanged, addedList, removedList));
		Monitor::Exit(m_gameObjects);
#endif
	}

#ifdef _EDITOR
	void Scene::Subscribe(SceneObjectsChangedEventHandler ^ func)
	{
		m_changeEvent += func;
	}

	void Scene::Unsubscribe(SceneObjectsChangedEventHandler ^ func)
	{
		m_changeEvent -= func;
	}
#endif

	GameObject^ Scene::Find(System::String ^ name)
	{
		for each(GameObject^ gameObject in GameObjects)
		{
			if (gameObject->Name == name)
				return gameObject;
		}
		return nullptr;
	}

	GameObject ^ Scene::Find(System::Guid guid)
	{
		for each(GameObject^ gameObject in GameObjects)
		{
			if (gameObject->m_guid == guid)
				return gameObject;
		}
		return nullptr;
	}

	GameObject ^ Scene::Find(thomas::object::GameObject * native)
	{
		for each(GameObject^ gameObject in GameObjects)
		{
			if (gameObject->nativePtr == native)
				return gameObject;
		}
		return nullptr;
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
#ifdef _EDITOR
	IEnumerable<GameObject^>^ Scene::GameObjectsSynced::get()
	{
		Monitor::Enter(m_gameObjects);
		auto l = gcnew List<GameObject^>(m_gameObjects);
		Monitor::Exit(m_gameObjects);
		return l;
	}
#endif

	IEnumerable<GameObject^>^ Scene::GameObjects::get()
	{
		return m_gameObjects;
	}
	List<GameObject^>^ Scene::GameObjectData::get() {
		return m_gameObjects;
	}
	void Scene::GameObjectData::set(List<GameObject^>^ val) {
		m_gameObjects = val;
	}

	generic<typename T>
		where T : Component
		inline List<T>^ Scene::getComponentsOfType()
		{
			Monitor::Enter(m_gameObjects);
			List<T>^ list = gcnew List<T>();
			for each(GameObject^ g in m_gameObjects)
			{
				for each(T c in g->Components)
				{
					list->Add(c);
				}
			}
			Monitor::Exit(m_gameObjects);
			return list;
		}
		List<System::Object^>^ Scene::getComponentsOfType(System::Type^ type)
		{
			Monitor::Enter(m_gameObjects);
			List<System::Object^>^ list = gcnew List<System::Object^>();
			for each(GameObject^ g in m_gameObjects)
			{
				for each(Component^ c in g->Components)
				{
					if(type->IsAssignableFrom(c->GetType()))
						list->Add(c);
				}
			}
			Monitor::Exit(m_gameObjects);
			return list;
		}
}

