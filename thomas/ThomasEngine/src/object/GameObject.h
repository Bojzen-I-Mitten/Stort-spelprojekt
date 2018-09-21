#pragma once
#pragma unmanaged
#include <thomas\object\GameObject.h>
#include <thomas\editor\EditorCamera.h>
#include <..\..\ThomasEngine\src\resource\Resources.h>

#pragma managed
#include <string>
#include <msclr\marshal_cppstd.h>
#include "../Scene.h"
#include "Object.h"
#include "Component.h"
#include "component\Transform.h"
#include "component\ScriptComponent.h"
#include "../attributes/CustomAttributes.h"
#include "component\RenderComponent.h"
#using "PresentationFramework.dll"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Threading;
using namespace System::Linq;

namespace ThomasEngine 
{
	public ref class GameObject : public Object
	{
		ObservableCollection<Component^> m_components;
		Transform^ m_transform;
		Scene^ scene;

		GameObject() : Object(new thomas::object::GameObject("gameobject")) 
		{
			m_name = "gameobject";

			System::Windows::Application::Current->Dispatcher->Invoke(gcnew Action(this, &GameObject::SyncComponents));
		}


		bool InitComponents(bool playing)
		{
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


	internal:

		String^ prefabPath;

		bool m_isDestroyed = false;
		System::Object^ m_componentsLock = gcnew System::Object();

		static void SerializeGameObject(String^ path, GameObject^ gObj);
		void SyncComponents() {
			System::Windows::Data::BindingOperations::EnableCollectionSynchronization(%m_components, m_componentsLock);
		}
		static System::IO::Stream^ SerializeGameObject(GameObject^ gObj);
		static GameObject^ DeSerializeGameObject(System::IO::Stream^ stream);

		void PostLoad(Scene^ scene)
		{
			this->scene = scene;
		}

		void PostInstantiate(Scene^ scene) {
			PostLoad(scene);
			scene->GameObjects->Add(this);
			for (int i = 0; i < m_transform->children->Count; i++) {
				m_transform->children[i]->gameObject->PostInstantiate(scene);
			}
		}


		static void InitGameObjects(bool playing) {
			bool completed;
			do {
				completed = true;
				for (int i = 0; i < Scene::CurrentScene->GameObjects->Count; i++) {
					completed = Scene::CurrentScene->GameObjects[i]->InitComponents(playing) && completed;
				}
			} while (!completed);
		}

		
				
		void Update()
		{
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

		void FixedUpdate()
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

		void RenderGizmos()
		{
			Monitor::Enter(m_componentsLock);
			for (int i = 0; i < m_components.Count; i++)
			{
				m_components[i]->OnDrawGizmos();
			}
			Monitor::Exit(m_componentsLock);
		}

		void RenderSelectedGizmos()
		{
			Monitor::Enter(m_componentsLock);
			for each(Component^ component in m_components)
			{
				component->OnDrawGizmosSelected();
			}
			Monitor::Exit(m_componentsLock);
		}

		void OnCollisionEnter(GameObject^ collider)
		{
			Monitor::Enter(m_componentsLock);

			for (int i = 0; i < m_components.Count; i++)
			{
				Component^ component = m_components[i];
				if (component->enabled)
					component->OnCollisionEnter(collider);
			}
			Monitor::Exit(m_componentsLock);
		}

	public:

		GameObject(String^ name) : Object(new thomas::object::GameObject(Utility::ConvertString(name))) 
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
		
		static GameObject^ CreatePrefab() {
			GameObject^ newGobj = gcnew GameObject();
			Transform^ t = newGobj->AddComponent<Transform^>();
			((thomas::object::GameObject*)newGobj->nativePtr)->m_transform = (thomas::object::component::Transform*)t->nativePtr;
			return newGobj;
		}




		property bool inScene {
			bool get() {
				return scene != nullptr;
			}
		}

		virtual void Destroy() override;

		property bool activeSelf
		{
			bool get()
			{
				return ((thomas::object::GameObject*)nativePtr)->m_activeSelf;
			}

			void set(bool value)
			{
				((thomas::object::GameObject*)nativePtr)->m_activeSelf = value;
				for (int i = 0; i < m_components.Count; i++)
				{
					
					Component^ component = m_components[i];
					if(component->initialized)
						component->enabled = value;
				}
			}
		}	

		String^ ToString() override
		{
			return Name;
		}

		[BrowsableAttribute(false)]
		[Xml::Serialization::XmlIgnoreAttribute]
		property Transform^ transform 
		{
			Transform^ get() 
			{
				return m_transform;
			}

			void set(Transform^ value) 
			{
				if (value)
				{
					m_transform = value;
					((thomas::object::GameObject*)nativePtr)->m_transform = (thomas::object::component::Transform*)m_transform->nativePtr;
				}
			}
		}

		property ObservableCollection<Component^>^ Components 
		{
			ObservableCollection<Component^>^ get() 
			{
				return %m_components;
			}
		}

		generic<typename T>
		where T : Component
		T AddComponent() 
		{
			Monitor::Enter(m_componentsLock);
			Type^ typ = T::typeid;

			T existingComponent = GetComponent<T>();
			if (existingComponent && typ->IsDefined(DisallowMultipleComponent::typeid, false))
			{	
				//LOG("Cannot add multiple instances of " << typ->Name);
				return T();
			}

			T component = (T)Activator::CreateInstance(T::typeid);
			((Component^)component)->setGameObject(this);
			m_components.Add((Component^)component);
			
			Monitor::Exit(m_componentsLock);
			return component;
		}

		generic<typename T>
		where T : Component
		T GetComponent()
		{
			List<T>^ tComponents = gcnew List<T>(Enumerable::OfType<T>(%m_components));
			if (tComponents->Count > 0)
				return tComponents[0];
			else
				return T();
		}

		Component^ GetComponent(Type^ type) {
			for (int i = 0; i < m_components.Count; i++) {
				if (m_components[i]->GetType() == type)
					return m_components[i];
			}
			return nullptr;
		}


		generic<typename T>
		where T : Component
		List<T>^ GetComponents()
		{
			List<T>^ list = gcnew List<T>(Enumerable::OfType<T>(%m_components));
			return list;
		}


		List<Component^>^ GetComponents(Type^ type) {
			List<Component^>^ tComponents = gcnew List<Component^>();
			for (int i = 0; i < m_components.Count; i++) {
				if (m_components[i]->GetType() == type)
					tComponents->Add(m_components[i]);
			}
			return tComponents;
		}


		bool HasComponentOfType(Type^ T)
		{
			for each(Component^ comp in m_components) {
				if (comp->GetType() == T)
					return true;
			}
			return false;
		}

		//generic<typename T>
		//where T : Component
		//List<T>^ GetComponents()
		//{
		//	return gcnew List<T>(Enumerable::OfType<T>(%m_components));
		//}
		//

		static List<GameObject^>^ GetAllGameObjects(bool includePrefabs) {
			List<GameObject^>^ gObjs = Object::GetObjectsOfType<GameObject^>();
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
		}

		static GameObject^ Find(String^ name) 
		{
			for each(GameObject^ gameObject in Scene::CurrentScene->GameObjects)
			{
				if (gameObject->Name == name)
					return gameObject;
			}
			return nullptr;
		}

		static GameObject^ CreatePrimitive(PrimitiveType type);

		bool GetActive()
		{
			return ((thomas::object::GameObject*)nativePtr)->GetActive();
		}

		void SetActive(bool active)
		{
			((thomas::object::GameObject*)nativePtr)->SetActive(active);
			activeSelf = active;

		}

		static GameObject^ Instantiate(GameObject^ original);
		static GameObject^ Instantiate(GameObject^ original, Transform^ parent);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation, Transform^ parent);


		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c)
		{
			for (int i = 0; i < m_components.Count; i++) {
				m_components[i]->gameObject = this;
			}
			m_transform = GetComponent<Transform^>();
		}
	};
}