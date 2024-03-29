
#pragma unmanaged
#include <thomas/editor/EditorCamera.h>
#include <thomas/object/ObjectHandler.h>
#include <thomas/utils/Utility.h>

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
		m_looseObjects(gcnew List<System::IDisposable^>()),
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

	void Scene::AwakeObjects(List<GameObject^>^ objects, bool playing)
	{
		// Call awake
		uint32_t initFlag = playing ? INIT_PLAYING_BIT : 0;
		for (int i = 0; i < objects->Count; i++)
		{
			GameObject^ g = objects[i];
			try
			{
				g->InitComponents(Comp::State::Awake, initFlag);
			}
			catch (Exception^ e)
			{
				// Need to remove failed object?!..
				Debug::LogException(e);
				Debug::LogError("Awake on object failed: " + (g != nullptr ? g->Name : "NULL"));
				DeleteInstant(g);
				objects->RemoveAt(i);
			}
		}
	}
	void Scene::EnableObjects(List<GameObject^>^ objects, bool playing)
	{
		// Verify non-editor components are activated
		uint32_t initFlag = playing ? INIT_PLAYING_BIT : 0;
		for (int i = 0; i < objects->Count; i++)
		{
			GameObject^ g = objects[i];
			try
			{
				g->InitComponents(Comp::State::Enabled, initFlag);
			}
			catch (Exception^ e)
			{
				// Need to remove failed object?!..
				Debug::LogException(e);
				Debug::LogError("Enable on object failed: " + (g != nullptr ? g->Name : "NULL"));
				DeleteInstant(g);
				objects->RemoveAt(i);
			}
		}
	}
	bool Scene::OnPlay()
	{
		ThomasWrapper::Thomas->SceneManagerRef->StoreTempCopy();
		try
		{
			constexpr uint32_t MAX_INITIATION_LOOPS = 1;

			// Start game objects
			AwakeObjects(m_gameObjects, true);
			SyncScene();							// Add objects created during awake (note: the objects will be enabled first)
			EnableObjects(m_gameObjects, true);

			for (uint32_t i = 0; i < MAX_INITIATION_LOOPS; i++)
				SyncScene();						// Add more objects if needed
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
	void Scene::AddLooseObject(System::IDisposable ^ object)
	{
		IssuedCommand cmd;
		cmd.m_cmd = Command::AddDisposable;
		cmd.m_obj = object;
		m_commandList->Add(cmd);
	}
	void Scene::DeleteInstant(GameObject ^ object)
	{
		if (!m_gameObjects->Remove(object))
		{
			Debug::LogWarning("Instant delete failed remove on: " + (object == nullptr ? "NULL" : object->Name));
		}
		try
		{
			object->OnDestroy();
		}
		catch (Exception^e)
		{
			Debug::LogError(e);
		}
		delete object;
	}


	void Scene::UnLoad()
	{
		// Flush command lists?
		Monitor::Enter(m_accessLock);
		for (int i = 0; i < m_gameObjects->Count; i++)
		{
			m_gameObjects[i]->OnDestroy();
			delete m_gameObjects[i];
		}
		for (int i = 0; i < m_looseObjects->Count; i++)
		{
			delete m_looseObjects[i];
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
			Debug::LogException(e);
			Debug::LogError("Failed loading scene: " + fullPath + "\n With following error: ");
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
			Debug::LogWarning("No objects in scene.");
			m_accessLock = gcnew Object();
			m_gameObjects = gcnew List<GameObject^>();
			//System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
		}
		int invalidCounter = 0;
		for (int i = 0; i < m_gameObjects->Count; i++)
		{
			if (m_gameObjects[i] == nullptr ||			// Deserialization failed.
				m_gameObjects[i]->transform == nullptr)	// Gameobject is invalid, can't be cleaned (objects must have transform)
			{
				m_gameObjects->RemoveAt(i--);
				invalidCounter++;
			}
		}
		if(invalidCounter)
			Debug::LogWarning("Object(s) not deserailized successfully, " + invalidCounter + " found to be NULL.");
	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}
	}

	void Scene::InitiateScene()
	{
		AwakeObjects(m_gameObjects, ThomasWrapper::IsPlaying());
		EnableObjects(m_gameObjects, ThomasWrapper::IsPlaying());
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

		for(int i = 0; i < m_commandList->Count; i++)
		{
			IssuedCommand c = m_commandList[i];
			switch (c.m_cmd)
			{
			case Command::Add:
				m_gameObjects->Add((GameObject^)c.m_obj);
#ifdef _EDITOR
				numChanged++;
				addedList->Add((GameObject^)c.m_obj);
#endif
				break;
			case Command::DisableRemove:
			{
				int index;
				// Verify delete call is valid.
				if (!InScene((GameObject^)c.m_obj, index))
				{
					Debug::LogWarning("Delete on object: " + ((GameObject^)c.m_obj)->Name + " was called twice");
				}
				else
				{
					// Disable object:
					ThomasWrapper::Selection->UnSelectGameObject((GameObject^)c.m_obj);
					m_gameObjects->RemoveAt(index);
					((GameObject^)c.m_obj)->OnDestroy();
#ifdef _EDITOR
					numChanged++;
					removedList->Add(((GameObject^)c.m_obj));
#endif
					// Wait to next frame for delete:
					IssuedCommand cmd;
					cmd.m_cmd = Command::Remove;
					cmd.m_obj = c.m_obj;
					m_commandSwapList->Add(cmd);
				}
			}
			break;
			case Command::Remove:
				// Destroy object
				delete c.m_obj;
				break;
			case Command::AddDisposable:
			{
				m_looseObjects->Add((IDisposable^)c.m_obj);
			}
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
		AwakeObjects(addedList, ThomasWrapper::IsPlaying());
		EnableObjects(addedList, ThomasWrapper::IsPlaying());
#ifdef _EDITOR
		if (numChanged)	// Trigger event 
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
	bool Scene::InScene(GameObject^ gObj, int &index)
	{
		index = -1;
		if (!gObj->SceneID == this->m_uniqueID)
			return false;
		for(int i = 0; i < m_gameObjects->Count; i++)
		{
			if (m_gameObjects[i] == gObj)
			{
				index = i;
				return true;
			}
		}
		return false;
	}
	/* Verify object is in scene
	*/
	bool Scene::InScene(GameObject ^ gObj)
	{
		int index;
		return InScene(gObj, index);
	}

	Vector3 Scene::CameraPosition::get() 
	{
		return Utility::Convert(thomas::editor::EditorCamera::Instance()->GetTransform()->GetLocalPosition());
	}

	void Scene::CameraPosition::set(Vector3 pos) 
	{

		thomas::editor::EditorCamera::Instance()->GetTransform()->SetLocalPosition(Utility::Convert(pos));
	}

	Vector3 Scene::CameraEuler::get() 
	{
		return Utility::Convert(thomas::editor::EditorCamera::Instance()->GetTransform()->GetLocalEulerAngles());
	}

	void Scene::CameraEuler::set(Vector3 euler) 
	{

		thomas::editor::EditorCamera::Instance()->GetTransform()->SetLocalRotation(euler.y, euler.x, euler.z);
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

	void Scene::AddLooseNative(thomas::object::Object* nativePtr)
	{
		AddLooseObject(gcnew DisposableNative<thomas::object::Object>(nativePtr));
	}

	generic<typename T>
	where T : Component
	inline List<T>^ Scene::getComponentsOfType()
	{
		Monitor::Enter(m_gameObjects);
		List<T>^ list = gcnew List<T>();
		for each(GameObject^ g in m_gameObjects)
		{
			// Check if valid object
			for each(Component^ c in g->Components)
			{
				if (T::typeid->IsAssignableFrom(c->GetType()))
					list->Add((T)c);
			}
		}
		Monitor::Exit(m_gameObjects);
		return list;
	}
	generic<typename T>
	where T : Component
	T Scene::findFirstComponent()
	{
		Monitor::Enter(m_gameObjects);
		for each(GameObject^ g in m_gameObjects)
		{
			for each(Component^ c in g->Components)
			{
				// Check if valid object
				if (T::typeid->IsAssignableFrom(c->GetType()))
				{
					Monitor::Exit(m_gameObjects);
					return (T)c;
				}
			}
		}
		Monitor::Exit(m_gameObjects);
		return T();
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

