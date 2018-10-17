#pragma unmanaged
#include <thomas\object\GameObject.h>
#include <thomas\object\ObjectHandler.h>
#pragma managed
#include "GameObject.h"
#include "Component.h"
#include "../Scene.h"
#include "component\RenderComponent.h"
#include "component\Transform.h"
#include "../ThomasSelection.h"
#include "../ThomasManaged.h"
#include "component\physics\BoxCollider.h"
#include "component\physics\SphereCollider.h"
#include "..\Debug.h"
#include "../resource/Model.h"
#include "../resource/Resources.h"
#include "../serialization/Serializer.h"
#using "PresentationFramework.dll"

using namespace System;
using namespace System::Threading;
namespace ThomasEngine {


	GameObject::GameObject() : Object(thomas::ObjectHandler::createNewGameObject("gameobject"))
	{
		m_name = "gameobject";

		if(ThomasWrapper::InEditor())
			System::Windows::Application::Current->Dispatcher->Invoke(gcnew Action(this, &GameObject::SyncComponents));
	}

	bool GameObject::getToBeStatic()
	{
		return m_toBeStatic;
	}

	GameObject::GameObject(String^ name) : Object(thomas::ObjectHandler::createNewGameObject(Utility::ConvertString(name)))
	{
		m_name = name;
		m_transform = AddComponent<Transform^>();
		((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;

		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());

		Scene::CurrentScene->GameObjects->Add(this);
		scene = Scene::CurrentScene;
		System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));

		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
	}

	bool GameObject::InitComponents(bool playing)
	{
		Monitor::Enter(m_componentsLock);
		bool completed = true;
		for(int i=0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			Type^ typ = component->GetType();
			bool executeInEditor = typ->IsDefined(ExecuteInEditor::typeid, false);
			if ((playing || executeInEditor) && !component->initialized) {
				completed = false;
				component->Initialize();
			}
		}
		Monitor::Exit(m_componentsLock);
		return completed;
	}

	thomas::object::GameObject* GameObject::Native::get() {
		return (thomas::object::GameObject*)nativePtr;
	}

	void GameObject::FlattenGameObjectTree(List<GameObject^>^ list, GameObject ^ root)
	{
		list->Add(root);
		for each(Transform^ child in root->transform->children)
		{
			GameObject::FlattenGameObjectTree(list, child->gameObject);
		}
	}

	void GameObject::PostLoad(Scene^ scene)
	{
		this->scene = scene;

	}

	void GameObject::PostInstantiate(Scene^ scene) {
		PostLoad(scene);
		scene->GameObjects->Add(this);
		for (int i = 0; i < m_transform->children->Count; i++) {
			m_transform->children[i]->gameObject->PostInstantiate(scene);
		}
	}
	void GameObject::InitGameObjects(bool playing) {
		bool completed;
		do {
			completed = true;
			for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; ++i) {
				GameObject^ gameObject = Scene::CurrentScene->GameObjects[i];
				completed = gameObject->InitComponents(playing) && completed;
			}
		} while (!completed);
	}

	thomas::object::Object* GameObject::setStatic()
	{


		thomas::object::Object* moved;
		nativePtr = thomas::ObjectHandler::setStatic(nativePtr, moved);

		m_toBeStatic = false;
		m_isStatic = true;

		return moved;
	}

	GameObject ^ GameObject::FindGameObjectFromNativePtr(thomas::object::GameObject* nativeptr)
	{
		if (nativeptr != nullptr)
		{

			for each (Object^ object in s_objects)
			{
				
				if (object->nativePtr == nativeptr)
					return static_cast<GameObject^>(object);
			}
		}

		return nullptr;
	}

	void GameObject::Update()
	{
		Monitor::Enter(m_componentsLock);

		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->initialized && component->enabled) {
				component->Update();
				component->UpdateCoroutines();
			}
				
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::FixedUpdate()
	{
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->initialized && component->enabled)
				component->FixedUpdate();
		}
		Monitor::Exit(m_componentsLock);
	}


	void GameObject::RenderGizmos()
	{
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++)
		{
			if(m_components[i]->enabled)
				m_components[i]->OnDrawGizmos();
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::RenderSelectedGizmos()
	{
		Monitor::Enter(m_componentsLock);
		for each(Component^ component in m_components)
		{
			component->OnDrawGizmosSelected();
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::Destroy()
	{
		if (m_isDestroyed)
			return;
		ThomasWrapper::Selection->UnSelectGameObject(this);
		m_isDestroyed = true;
		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++) {
			m_components[i]->Destroy();
			i--;
		}
		Object::Destroy();
		m_components.Clear();
		Monitor::Exit(m_componentsLock);
		Scene::CurrentScene->GameObjects->Remove(this);
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
	}

	GameObject ^ ThomasEngine::GameObject::CreatePrimitive(PrimitiveType type)
	{
		// This function has been hooked by GameObject manager, this does nothng
		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
		return gameObject;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original)
	{
		if(!original){
			Debug::LogError("Object to instantiate is null");
			return nullptr;
		}
		if (original->m_isDestroyed)
		{
			Debug::LogError("Trying to instantiate destroyed object.");
			return nullptr;
		}
		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		GameObject^ clone = nullptr;
		if (original->prefabPath != nullptr) {
			clone = Resources::LoadPrefab(original->prefabPath, true);
		}
		else
		{
			try {
				Newtonsoft::Json::Linq::JArray^ serialized = Serializer::SerializeGameObject(original);
				clone = Serializer::DeserializeGameObject(serialized);
			}
			catch (Exception^ e)
			{
				String^ msg = "Failed to instantiate gameObject: " + original->Name + " error: " + e->Message;
				Debug::LogError(msg);
			}

		}
		
		if (clone) {
			clone->transform->SetParent(nullptr, true);
			clone->PostInstantiate(Scene::CurrentScene);
			clone->prefabPath = nullptr;
		}
			
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original);
		if(clone)
			clone->transform->SetParent(parent, true);
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation)
	{
		GameObject^ clone = Instantiate(original);
		if(clone){
			clone->transform->position = position;
			clone->transform->rotation = rotation;
		}
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original, parent);
		if (clone) {
			clone->transform->position = position;
			clone->transform->rotation = rotation;
		}
		return clone;
	}

	GameObject^ GameObject::CreatePrefab() {
		GameObject^ newGobj = gcnew GameObject();
		Transform^ t = newGobj->AddComponent<Transform^>();
		((thomas::object::GameObject*)newGobj->nativePtr)->m_transform = (thomas::object::component::Transform*)t->nativePtr;
		return newGobj;
	}

	void GameObject::toBeStatic()
	{
		m_toBeStatic = !m_toBeStatic;
	}


	generic<typename T>
	where T : Component
	T GameObject::AddComponent()
	{
		Monitor::Enter(m_componentsLock);
		Type^ typ = T::typeid;

		T existingComponent = GetComponent<T>();
		if (existingComponent && typ->IsDefined(DisallowMultipleComponent::typeid, false))
		{
			//LOG("Cannot add multiple instances of " << typ->Name);
			return T();
		}

		T component = (T)System::Activator::CreateInstance(T::typeid);
		((Component^)component)->setGameObject(this);
		m_components.Add((Component^)component);

		Monitor::Exit(m_componentsLock);
		return component;
	}

	generic<typename T>
	where T : Component
	T GameObject::GetComponent()
	{
		List<T>^ tComponents = gcnew List<T>(System::Linq::Enumerable::OfType<T>(%m_components));
		if (tComponents->Count > 0)
			return tComponents[0];
		else
			return T();
	}


	Component^ GameObject::GetComponent(Type^ type) {
		for (int i = 0; i < m_components.Count; i++) {
			if (m_components[i]->GetType() == type)
				return m_components[i];
		}
		return nullptr;
	}

	generic<typename T>
	where T : Component
	List<T>^ GameObject::GetComponents()
	{
		List<T>^ list = gcnew List<T>(System::Linq::Enumerable::OfType<T>(%m_components));
		return list;
	}

	List<Component^>^ GameObject::GetComponents(Type^ type) {
		List<Component^>^ tComponents = gcnew List<Component^>();
		for (int i = 0; i < m_components.Count; i++) {
			if (m_components[i]->GetType() == type)
				tComponents->Add(m_components[i]);
		}
		return tComponents;
	}
	bool GameObject::HasComponentOfType(Type^ T)
	{
		for each(Component^ comp in m_components) {
			if (comp->GetType() == T)
				return true;
		}
		return false;
	}

	List<GameObject^>^ GameObject::GetAllGameObjects(bool includePrefabs) {
		List<GameObject^>^ gObjs = ThomasEngine::Object::GetObjectsOfType<GameObject^>();

		for (int i = 0; i < gObjs->Count; i++) {
			if (!gObjs[i]->inScene) {
				if (!includePrefabs || !gObjs[i]->prefabPath) {
					gObjs->RemoveAt(i);
					i--;
				}
				
			}

		}
		return gObjs;
	}

	GameObject^ GameObject::Find(String^ name)
	{
		for each(GameObject^ gameObject in Scene::CurrentScene->GameObjects)
		{
			if (gameObject->Name == name)
				return gameObject;
		}
		return nullptr;
	}

	bool GameObject::GetActive()
	{
		if((thomas::object::GameObject*)nativePtr != nullptr)
			return ((thomas::object::GameObject*)nativePtr)->GetActive();
		return false;
	}

	void GameObject::SetActive(bool active)
	{
		activeSelf = active;
		((thomas::object::GameObject*)nativePtr)->SetActive(active);
		

	}

	bool GameObject::activeSelf::get()
	{
		return ((thomas::object::GameObject*)nativePtr)->m_activeSelf;
	}

	bool GameObject::staticSelf::get()
	{
		return m_toBeStatic;
	}

	void GameObject::staticSelf::set(bool value)
	{
		m_toBeStatic = value;
	}

	void GameObject::activeSelf::set(bool value)
	{
		if (value == activeSelf)
			return;
		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->m_firstEnable && component->enabled) {
				if (value)
					component->OnEnable();
				else
					component->OnDisable();
			}
				
		}
		((thomas::object::GameObject*)nativePtr)->m_activeSelf = value;
		OnPropertyChanged("activeSelf");
	}

	String^ GameObject::Name::get() {
		if (inScene)
			return m_name;
		else
			return m_name + " (prefab)";
	}
	void GameObject::Name::set(System::String^ value)
	{
		m_name = value;
		OnPropertyChanged("Name");
	}

	Transform^ GameObject::transform::get()
	{
		return m_transform;
	}

	void GameObject::transform::set(Transform^ value)
	{
		if (value)
		{
			m_transform = value;
			((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;
		}
	}

	void GameObject::SyncComponents() {
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(%m_components, m_componentsLock);
	}

	void GameObject::OnDeserialized(System::Runtime::Serialization::StreamingContext c)
	{
		for (int i = 0; i < m_components.Count; i++) {
			m_components[i]->gameObject = this;
		}
		transform = GetComponent<Transform^>();
		nativePtr->SetName(Utility::ConvertString(m_name));
	}


	bool GameObject::GetStatic()
	{
		return m_isStatic;
	}
}