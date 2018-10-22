#pragma unmanaged
#include <thomas\object\GameObject.h>
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


	GameObject::GameObject() : Object(new thomas::object::GameObject("gameobject"))
	{
		m_name = "gameobject";
#ifdef _EDITOR
		if(ThomasWrapper::InEditor())
			System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif
	}

	GameObject::GameObject(String^ name) : Object(new thomas::object::GameObject(Utility::ConvertString(name)))
	{
		m_name = name;
		m_transform = AddComponent<Transform^>();
		((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;

		Monitor::Enter(ThomasWrapper::CurrentScene->GetGameObjectsLock());
		// Add to scene
		ThomasWrapper::CurrentScene->GameObjects->Add(this);
		m_scene_id = ThomasWrapper::CurrentScene->ID();
#ifdef _EDITOR
		System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif

		Monitor::Exit(ThomasWrapper::CurrentScene->GetGameObjectsLock());
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
		m_scene_id = ThomasWrapper::CurrentScene->ID();
	}

	void GameObject::PostInstantiate(Scene^ scene) {
		PostLoad(scene);
		scene->GameObjects->Add(this);
		for each(Transform^ child in m_transform->children)
			child->gameObject->PostInstantiate(scene);
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

	GameObject::~GameObject()
	{
		Delete();
	}

	void GameObject::Delete()
	{
		for (int i = 0; i < m_components.Count; i++)
		{
			m_components[i]->OnDestroy();
			delete m_components[i];	// Begone you foul clr!!!!
		}
		m_components.Clear();
	}

	void GameObject::Destroy()
	{
		
		if (m_isDestroyed)
			return;
		ThomasWrapper::Selection->UnSelectGameObject(this);
		m_isDestroyed = true;
		
		// Remove object
		Monitor::Enter(ThomasWrapper::CurrentScene->GetGameObjectsLock());
		ThomasWrapper::CurrentScene->GameObjects->Remove(this);
		Monitor::Exit(ThomasWrapper::CurrentScene->GetGameObjectsLock());
		// Destroy
		Monitor::Enter(m_componentsLock);
		Delete();
		Monitor::Exit(m_componentsLock);
		Object::Destroy();
	}

	GameObject ^ ThomasEngine::GameObject::CreatePrimitive(PrimitiveType type)
	{
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
		Monitor::Enter(ThomasWrapper::CurrentScene->GetGameObjectsLock());
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
			clone->PostInstantiate(ThomasWrapper::CurrentScene);
			clone->prefabPath = nullptr;
		}
			
		Monitor::Exit(ThomasWrapper::CurrentScene->GetGameObjectsLock());
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
		for each(GameObject^ gameObject in ThomasWrapper::CurrentScene->GameObjects)
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
			if(m_components[i])
				m_components[i]->gameObject = this;
		}
		transform = GetComponent<Transform^>();
		nativePtr->SetName(Utility::ConvertString(m_name));
	}

}