#pragma unmanaged
#include <thomas\object\GameObject.h>
#include <thomas\AutoProfile.h>
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
#include "../SceneSurrogate.h"
#include "../resource/Model.h"
#include "../resource/Resources.h"
#using "PresentationFramework.dll"
using namespace System;
using namespace System::Threading;
namespace ThomasEngine {


	GameObject::GameObject() : Object(new thomas::object::GameObject("gameobject"))
	{
		m_name = "gameobject";

		System::Windows::Application::Current->Dispatcher->Invoke(gcnew Action(this, &GameObject::SyncComponents));
	}

	GameObject::GameObject(String^ name) : Object(new thomas::object::GameObject(Utility::ConvertString(name)))
	{
		m_name = name;
		m_transform = AddComponent<Transform^>();
		((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;

		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());

		Scene::CurrentScene->GameObjects->Add(this);
		scene = Scene::CurrentScene;
		System::Windows::Application::Current->Dispatcher->Invoke(gcnew Action(this, &GameObject::SyncComponents));

		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
	}

	bool GameObject::InitComponents(bool playing)
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		Monitor::Enter(m_componentsLock);
		bool completed = true;
		for each(Component^ component in m_components)
		{
			Type^ typ = component->GetType();
			if ((playing || typ->IsDefined(ExecuteInEditor::typeid, false)) && !component->initialized) {
				completed = false;
				component->Initialize();
			}
		}
		Monitor::Exit(m_componentsLock);
		return completed;
	}

	void GameObject::PostLoad(Scene^ scene)
	{
		this->scene = scene;

	}

	void GameObject::PostInstantiate(Scene^ scene) {
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		PostLoad(scene);
		scene->GameObjects->Add(this);
		for (int i = 0; i < m_transform->children->Count; i++) {
			m_transform->children[i]->gameObject->PostInstantiate(scene);
		}
	}
	void GameObject::InitGameObjects(bool playing) {
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		bool completed;
		do {
			completed = true;
			for each(GameObject^ gameObject in Scene::CurrentScene->GameObjects) {
				completed = gameObject->InitComponents(playing) && completed;
			}
		} while (!completed);
	}

	void GameObject::Update()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		Monitor::Enter(m_componentsLock);

		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->enabled) 
			{
				component->Update();
				component->UpdateCoroutines();
			}
				
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::FixedUpdate()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->initialized && component->enabled)
				component->FixedUpdate();
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::OnCollisionEnter(GameObject^ collider)
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		Monitor::Enter(m_componentsLock);

		for (int i = 0; i < m_components.Count; i++)
		{
			Component^ component = m_components[i];
			if (component->enabled)
				component->OnCollisionEnter(collider);
		}
		Monitor::Exit(m_componentsLock);
	}

	void GameObject::RenderGizmos()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++)
		{
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
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		if (m_isDestroyed)
			return;
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
		ThomasWrapper::Selection->UnSelectGameObject(this);
		Scene::CurrentScene->GameObjects->Remove(this);
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
	}

	GameObject ^ ThomasEngine::GameObject::CreatePrimitive(PrimitiveType type)
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
		return gameObject;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original)
	{
		if(!original){
			Debug::Log("Object to instantiate is null");
			return nullptr;
		}
		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		GameObject^ clone;
		if (original->prefabPath != nullptr) {
			clone = Resources::LoadPrefab(original->prefabPath, true);
		}
		else
		{
			System::IO::Stream^ serialized = SerializeGameObject(original);
			clone = DeSerializeGameObject(serialized);
		}
		
		if (clone) {
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
			clone->transform->parent = parent;
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
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		GameObject^ newGobj = gcnew GameObject();
		Transform^ t = newGobj->AddComponent<Transform^>();
		((thomas::object::GameObject*)newGobj->nativePtr)->m_transform = (thomas::object::component::Transform*)t->nativePtr;
		return newGobj;
	}

	void GameObject::SerializeGameObject(String ^ path, GameObject ^ gObj)
	{
		using namespace System::Runtime::Serialization;
		DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
		auto list = Component::GetAllComponentTypes();
		list->Add(SceneResource::typeid);
		serializerSettings->KnownTypes = list;
		serializerSettings->PreserveObjectReferences = true;
		serializerSettings->DataContractSurrogate = gcnew SceneSurrogate();
		DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

		Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
		settings->Indent = true;
				
		Xml::XmlWriter^ writer = Xml::XmlWriter::Create(path, settings);
		serializer->WriteObject(writer, gObj);
		writer->Close();
	}

	System::IO::Stream^ GameObject::SerializeGameObject(GameObject ^ gObj)
	{
		using namespace System::Runtime::Serialization;
		DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
		auto list = Component::GetAllComponentTypes();
		list->Add(SceneResource::typeid);
		serializerSettings->KnownTypes = list;
		serializerSettings->PreserveObjectReferences = true;
		serializerSettings->DataContractSurrogate = gcnew SceneSurrogate();
		DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

		System::IO::Stream^ stream = gcnew System::IO::MemoryStream();
		serializer->WriteObject(stream, gObj);
		return stream;
	}

	GameObject ^ GameObject::DeSerializeGameObject(System::IO::Stream ^ stream)
	{
		try {
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
			auto list = Component::GetAllComponentTypes();
			list->Add(SceneResource::typeid);
			serializerSettings->KnownTypes = list;
			serializerSettings->PreserveObjectReferences = true;
			serializerSettings->DataContractSurrogate = gcnew SceneSurrogate();
			DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

			stream->Seek(0, System::IO::SeekOrigin::Begin);
			GameObject^ gObj = (GameObject^)serializer->ReadObject(stream);
			gObj->PostLoad(nullptr);
			return gObj;
		}
		catch (Exception^ e) {
			std::string msg("Failed to load gameObject, msg: " + Utility::ConvertString(e->Message));
			LOG(msg);
			return nullptr;
		}
		

	}


	generic<typename T>
	where T : Component
	T GameObject::AddComponent()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::allocation)
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
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		List<T>^ tComponents = gcnew List<T>(System::Linq::Enumerable::OfType<T>(%m_components));
		if (tComponents->Count > 0)
			return tComponents[0];
		else
			return T();
	}


	Component^ GameObject::GetComponent(Type^ type) {
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
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
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
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
		if (includePrefabs)
			return gObjs;
		else
		{
			for (int i = 0; i < gObjs->Count; i++) {
				if (!gObjs[i]->inScene) {
					gObjs->RemoveAt(i);
					i--;
				}

			}
			return gObjs;
		}
		return nullptr;
	}

	GameObject^ GameObject::Find(String^ name)
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		for each(GameObject^ gameObject in Scene::CurrentScene->GameObjects)
		{
			if (gameObject->Name == name)
				return gameObject;
		}
		return nullptr;
	}

	bool GameObject::GetActive()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		return ((thomas::object::GameObject*)nativePtr)->GetActive();
	}

	void GameObject::SetActive(bool active)
	{
		((thomas::object::GameObject*)nativePtr)->SetActive(active);
		activeSelf = active;

	}

	bool GameObject::activeSelf::get()
	{
		return ((thomas::object::GameObject*)nativePtr)->m_activeSelf;
	}

	void GameObject::activeSelf::set(bool value)
	{
		((thomas::object::GameObject*)nativePtr)->m_activeSelf = value;
		for (int i = 0; i < m_components.Count; i++)
		{

			Component^ component = m_components[i];
			if (component->initialized)
				component->enabled = value;
		}
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
		m_transform = GetComponent<Transform^>();
		nativePtr->SetName(Utility::ConvertString(m_name));
	}

}