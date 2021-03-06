#pragma unmanaged
#include <thomas\object\GameObject.h>
#include <thomas\object\ObjectHandler.h>
#include <thomas/utils/Utility.h>
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
		Object(new thomas::object::GameObject("gameobject"))
	{
		m_name = "gameobject";
#ifdef _EDITOR
		//if(ThomasWrapper::IsEditorBuild())
		//	System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif
	}

	GameObject::GameObject(String^ name) : 
		Object(new thomas::object::GameObject(Utility::ConvertString(name)))
	{
		m_name = name;
		Tag = "";
		Layer = 0;
		m_transform = AddComponent<Transform^>();
		((thomas::object::GameObject*)nativePtr)->SetTransform((thomas::object::component::Transform*)m_transform->nativePtr);

		// Add to scene
		ThomasWrapper::CurrentScene->CreateObject(this);
		m_scene_id = ThomasWrapper::CurrentScene->ID();
#ifdef _EDITOR
		//if (ThomasWrapper::IsEditorBuild())
		//	System::Windows::Application::Current->Dispatcher->BeginInvoke(gcnew Action(this, &GameObject::SyncComponents));
#endif
	}

	/* Initiation process for activating the object.
	*/
	void GameObject::InitComponents(Comp::State s, uint32_t InitBits)
	{
		// Don't enable de-activated objects.
		if (s != Comp::State::Awake && !this->GetActive())
			return;

		// Activate
		for (int i = 0; i < m_components.Count; i++)
		{
			m_components[i]->InitComponent(s, InitBits);
		}
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
		CleanComponents();
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
		for each (Component^ comp in m_components)
		{
			comp->OnDestroy();
			delete comp;	// Begone you foul Clr!!!!
		}
		m_components.Clear();
	}
	void GameObject::OnActivate()
	{
		uint32_t bits = INIT_EXPLICIT_CALL_BIT | (ThomasWrapper::IsPlaying() ? INIT_PLAYING_BIT : 0u);
		InitComponents(Comp::State::Enabled, bits);
		for each (Transform^ g in Children)
			g->gameObject->OnActivate();
	}
	void GameObject::OnDeactivate()
	{
		uint32_t bits = INIT_EXPLICIT_CALL_BIT | (ThomasWrapper::IsPlaying() ? INIT_PLAYING_BIT : 0u);
		InitComponents(Comp::State::Disabled, bits);
		for each (Transform^ g in Children)
			g->gameObject->OnDeactivate();
	}
	void GameObject::OnDestroy()
	{
		for each (Component^ comp in m_components)
		{
			// Disable
			comp->OnParentDestroy(this);
			if (comp->ComponentState != Comp::Uninitialized)
				comp->enabled = false; // comp->Disable()
		}
	}
	bool GameObject::RemoveComponent(Component ^ comp)
	{
		// Remove single component
		bool success = false;
		if (m_components.Remove(comp))
		{
			comp->OnParentDestroy(this);

			Type^ typ = comp->GetType();
			bool executeInEditor = typ->IsDefined(ExecuteInEditor::typeid, false);

			if (comp->ComponentState == Comp::Enabled)
				comp->enabled = false; // comp->Disable()
			//if(comp->m_state != component::Uninitialized) // Always true.
			comp->OnDestroy();
			delete comp;	// Begone you foul Clr!!!!
			success = true;
			m_changeEvent(this, gcnew ComponentsChangedArgs());
		}
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
		}
	}

	void GameObject::FixedUpdate()
	{
		try {
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
		}
	}


	void GameObject::RenderGizmos()
	{
		for (int i = 0; i < m_components.Count; i++)
		{
			if(m_components[i]->enabled)
				m_components[i]->OnDrawGizmos();
		}
	}

	void GameObject::RenderSelectedGizmos()
	{
		for each(Component^ component in m_components)
		{
			component->OnDrawGizmosSelected();
		}
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
		((thomas::object::GameObject*)newGobj->nativePtr)->SetTransform((thomas::object::component::Transform*)t->nativePtr);
		return newGobj;
	}
	
	void GameObject::SetComponentIndex(Component ^ c, uint32_t index)
	{
		// Clamp to valid indices
		if (index < 1u) index = 1;
		if (index >= (uint32_t)m_components.Count) index = m_components.Count - 1;

		for (uint32_t i = 0; i < (uint32_t)m_components.Count; i++) {
			if (m_components[i] == c)
			{
				if (i == index) break;
				// Found comp. insert at new index.
				m_components[i] = m_components[index];
				m_components[index] = c;
				m_changeEvent(this, gcnew ComponentsChangedArgs());
				break;
			}
		}
	}
	uint32_t GameObject::GetComponentIndex(Component ^ c)
	{
		uint32_t index = UINT_MAX;
		try
		{
			for (uint32_t i = 0; i < (uint32_t)m_components.Count; i++) {
				if (m_components[i] == c)
				{
					return i;
				}
			}
		}
		catch (System::Exception^ e)
		{
			Debug::LogWarning("GetComponentIndex failed due to wpf thread active in serial state in editor.");
		}
		return index;
	}
	generic<typename T>
	where T : Component
	T GameObject::AddComponent()
	{
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
		Component^ comp = (Component^)component;

		comp->setGameObject(this);
		m_components.Add(comp);

		// Wake up
		uint32_t BITS = ThomasWrapper::IsPlaying() ? INIT_PLAYING_BIT : 0;
		comp->InitComponent(Comp::State::Awake, BITS);
		if(this->GetActive())
			comp->InitComponent(Comp::State::Enabled, BITS);
		m_changeEvent(this, gcnew ComponentsChangedArgs());
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
		// Activate/Deactivate object. Calls: SetActive(true/false)
		if (active)
		{
			// Activate object before initiation trigger (prevents initiation process to fail on inactive)
			((thomas::object::GameObject*)nativePtr)->SetActive(true);
			OnActivate();
		}
		else
		{
			OnDeactivate();
			// Deactivate object after trigger (prevents initiation process to fail on inactive)
			((thomas::object::GameObject*)nativePtr)->SetActive(false);
		}


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
		if(m_transform == nullptr)
			// Serialization activation
			((thomas::object::GameObject*)nativePtr)->SetActive(value);
		else
			// If editor make it trigger full activation part.
			SetActive(value);
#else
		((thomas::object::GameObject*)nativePtr)->SetActive(value);
#endif
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
	GameObject^ GameObject::Parent::get()
	{
		return transform->parent != nullptr ? transform->parent->gameObject : nullptr;
	}

	void GameObject::transform::set(Transform^ value)
	{
		if (value)
		{
			m_transform = value;
			((thomas::object::GameObject*)nativePtr)->SetTransform((thomas::object::component::Transform*)m_transform->nativePtr);
		}
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
	void GameObject::Subscribe(ComponentsChangedEventHandler ^ e)
	{
		m_changeEvent += e;
	}
	void GameObject::UnSubscribe(ComponentsChangedEventHandler ^ e)
	{
		m_changeEvent -= e;
	}
}