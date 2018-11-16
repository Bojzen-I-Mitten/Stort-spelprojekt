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


	GameObject::GameObject() : 
		Object(thomas::ObjectHandler::Instance().createNewGameObject("gameobject"))
	{
		m_name = "gameobject";
#ifdef _EDITOR
		if(ThomasWrapper::InEditor())
			System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif
	}

	GameObject::GameObject(String^ name) : 
		Object(thomas::ObjectHandler::Instance().createNewGameObject(Utility::ConvertString(name)))
	{
		m_name = name;
		Tag = "";
		Layer = 0;
		m_transform = AddComponent<Transform^>();
		((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;

		// Add to scene
		ThomasWrapper::CurrentScene->CreateObject(this);
		m_scene_id = ThomasWrapper::CurrentScene->ID();
#ifdef _EDITOR
		if (ThomasWrapper::InEditor())
			System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif
	}
	void GameObject::InitComponents(Comp::State s, bool playing)
	{
		/* Object initiation Process.
		 * 1. Awake()
		 * 2. if 'Active'
			Enable(), Start()

		 * Editor is a separate case where not all objects are enabled 
		*/
		if (s != Comp::State::Awake && !this->GetActive())
			return;	// Don't enable de-activated objects.

		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ c = m_components[i];
			// Logic over state s to enter... Should be separate functions at some point...
			switch (s)
			{
			case Comp::State::Awake:
			{
#ifdef _EDITOR														
				// If editor state: don't initiate all components
				if (!(playing || c->enableInEditor())) continue;
				// Ensure component isn't initialized
				if (c->ComponentState != Comp::State::Uninitialized) continue;
#else		
				assert(c->State == Comp::State::Uninitialized);
#endif			
				c->Awake();
			}
				break;
			case Comp::State::Enabled:
			{
				// If Component isn't activated 
				if (!c->Activated || 
					!(c->ComponentState == Comp::State::Awake || c->ComponentState == Comp::State::Disabled))
					continue;
				c->Enable();
			}
				break;
			case Comp::State::Disabled:
				if (!c->Activated || c->ComponentState != Comp::State::Enabled)
					continue;	// Ignore inactive components.
				c->Disable();
				break;
			case Comp::State::Uninitialized:
			case Comp::State::EndState:
			default:
				assert(false); // Don't
				break;
			}
		}
		Monitor::Exit(m_componentsLock);
	}

	thomas::object::GameObject* GameObject::Native::get() {
		return reinterpret_cast<thomas::object::GameObject*>(nativePtr);
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

	void GameObject::CleanComponents()
	{
		for (int i = 0; i < m_components.Count; i++)
		{
			if (m_components[i] == nullptr)
			{
				if (i == 0)
					throw gcnew Exception("Corrupt Transform Component in GameObject. Object is invalid.");
				Debug::LogWarning("Corrupt Component at index: " + i + " in GameObject: " + m_name);
				m_components.RemoveAt(i--);
			}
		}
	}

	void GameObject::PostInstantiate(Scene^ scene) {
		PostLoad(scene);
		scene->CreateObject(this);
		for each(Transform^ child in m_transform->children)
			child->gameObject->PostInstantiate(scene);
	}
 

	GameObject::~GameObject()
	{
		// Final delete of object
		Monitor::Enter(m_componentsLock);
		for each (Component^ comp in m_components)
		{
			comp->OnDestroy();
			delete comp;	// Begone you foul Clr!!!!
		}
		m_components.Clear();
		Monitor::Exit(m_componentsLock);
	}
	void GameObject::OnActivate()
	{
		// Activate components
		InitComponents(Comp::State::Enabled, ThomasWrapper::IsPlaying());
		for each (Transform^ g in Children)
			g->gameObject->OnActivate();
	}
	void GameObject::OnDeactivate()
	{
		InitComponents(Comp::State::Disabled, ThomasWrapper::IsPlaying());
		for each (Transform^ g in Children)
			g->gameObject->OnDeactivate();
	}
	void GameObject::OnDestroy()
	{
		// OnDisable^
		for each (Component^ comp in m_components)
		{
			// Disable
			comp->OnParentDestroy(this);
			if (comp->ComponentState != Comp::Uninitialized)
				comp->OnDisable();
		}
	}
	bool GameObject::RemoveComponent(Component ^ comp)
	{
		// Remove single component
		bool success = false;
		Monitor::Enter(m_componentsLock);
		if (m_components.Remove(comp))
		{
			comp->OnParentDestroy(this);

			Type^ typ = comp->GetType();
			bool executeInEditor = typ->IsDefined(ExecuteInEditor::typeid, false);

			if (comp->ComponentState == Comp::Enabled)
				comp->OnDisable();
			//if(comp->m_state != component::Uninitialized) // Always true.
			comp->OnDestroy();
			delete comp;	// Begone you foul Clr!!!!
			success = true;
		}
		Monitor::Exit(m_componentsLock);
		return success;
	}
	void GameObject::Destroy()
	{
		List<Transform^>^ children = gcnew List<Transform^>(transform->children);
		for each(Transform^ child in children)
		{
			Destroy(child->gameObject);
		}
		DestroySelf();
	}
	void GameObject::DestroySelf()
	{
		ThomasWrapper::CurrentScene->DestroyObject(this);
	}

	bool GameObject::MakeStatic()
	{
		return m_makeStatic;
	}

	bool GameObject::MakeDynamic()
	{
		return m_makeDynamic;
	}

	bool GameObject::MoveStaticGroup()
	{
		return ((thomas::object::GameObject*)nativePtr)->GetMoveStaticGroup();
	}
	
	thomas::object::Object* GameObject::setStatic()
	{
		thomas::object::Object* moved;

		nativePtr = thomas::ObjectHandler::Instance().setStatic(nativePtr, moved);

		m_makeStatic = false;

		return moved;
	}

	thomas::object::Object * GameObject::moveStaticGroup()
	{
		thomas::object::Object* moved;

		nativePtr = thomas::ObjectHandler::Instance().moveStaticGroup(nativePtr, moved);

		return moved;
	}

	thomas::object::Object * GameObject::setDynamic()
	{
		thomas::object::Object* moved;

		nativePtr = thomas::ObjectHandler::Instance().setDynamic(nativePtr, moved);

		m_makeDynamic = false;

		return moved;
	}


	void GameObject::Update()
	{
		Monitor::Enter(m_componentsLock);

		try {
			for (int i = 0; i < m_components.Count; i++)
			{
				Component^ component = m_components[i];
				if (component->enabled) {
					component->Update();
					component->UpdateCoroutines();
				}
			}
		}
		catch (Exception^ e) {
			Debug::LogException(e);
		}
		finally{
			Monitor::Exit(m_componentsLock);
		}
	}

	void GameObject::FixedUpdate()
	{
		try {
			Monitor::Enter(m_componentsLock);
			for (int i = 0; i < m_components.Count; i++)
			{
				Component^ component = m_components[i];
				if (component->enabled)
					component->FixedUpdate();
			}
		}
		catch (Exception^ e) {
			Debug::LogError("Fixed component update failed with exception: " + e->Message);
		}
		finally{
			Monitor::Exit(m_componentsLock);
		}
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
	bool GameObject::IsPrefab()
	{
		return prefabPath != nullptr && (inScene == 0);
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
		GameObject^ clone = nullptr;
		if (original->IsPrefab()) {
			// Generate a new prefab
			clone = Resources::CreatePrefab(original->prefabPath);
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
		}
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
	
	GameObject^ GameObject::CreateEmptyPrefab() {
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
		if (component == nullptr)
		{
			Debug::LogWarning("Component failed to instantiate of type " + (T::typeid));
			return T();
		}

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

	generic<typename T>
		where T : Component
	T GameObject::GetComponent(void* nativePtr)
	{
		for each (Component^ c in m_components)
		{
			if (c->nativePtr == nativePtr)
				return (T)c;
		}
		return T();	// nullptr
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
	
	GameObject^ GameObject::Find(String^ name)
	{
		return ThomasWrapper::CurrentScene->Find(name);
	}

	bool GameObject::GetActive()
	{
		if((thomas::object::GameObject*)nativePtr != nullptr)
			return ((thomas::object::GameObject*)nativePtr)->GetActive();
		return false;
	}

	void GameObject::SetActive(bool active)
	{
		if (active == GetActive())
			return;
		// Activate object
		((thomas::object::GameObject*)nativePtr)->SetActive(active);
		
		if (active)
			OnActivate();
		else
			OnDeactivate();


		// Trigger change
		OnPropertyChanged("activeSelf");
	}

	int GameObject::Layer::get()
	{
		return ((thomas::object::GameObject*)nativePtr)->GetLayer();
	}

	void GameObject::Layer::set(int value)
	{
		((thomas::object::GameObject*)nativePtr)->SetLayer(value);
	}

	bool GameObject::activeSelf::get()
	{
		return GetActive();
	}

	UINT GameObject::GroupIDSelf::get()
	{
		return ((thomas::object::GameObject*)nativePtr)->GetGroupID();
	}

	void GameObject::GroupIDSelf::set(UINT value)
	{

		((thomas::object::GameObject*)nativePtr)->ChangeGroupID(value);
	}

	bool GameObject::staticSelf::get()
	{
		return (((thomas::object::GameObject*)nativePtr)->GetStatic() || m_makeStatic) && !m_makeDynamic;
	}

	void GameObject::staticSelf::set(bool state)
	{
		if (state)
		{
			// If box was checked and it's not static, we flag that 
			// Object is going to be moved next frame
			if (!((thomas::object::GameObject*)nativePtr)->GetStatic())
				m_makeStatic = true;
		}
		else // if state is false, somebody just unchecked the box
		{
			if (!((thomas::object::GameObject*)nativePtr)->GetDynamic())
			{
				m_makeDynamic = true;
			}
		}
	}

	void GameObject::activeSelf::set(bool value)
	{
#ifdef _EDITOR
		// If editor make it trigger full activation part.
		// (variable is not enabled during play mode)
		if (!ThomasWrapper::IsPlaying())
			SetActive(value);
		else
#endif
		{
			((thomas::object::GameObject*)nativePtr)->SetActive(value);
		}
	}

	String^ GameObject::Name::get() {
		if (!IsPrefab())
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

	IEnumerable<Transform^>^ GameObject::Children::get()
	{
		return transform->children;
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
		transform = GetComponent<Transform^>();
		nativePtr->SetName(Utility::ConvertString(m_name));
		for (int i = 0; i < m_components.Count; i++) {
			if(m_components[i])
				m_components[i]->gameObject = this;
		}
	}
}