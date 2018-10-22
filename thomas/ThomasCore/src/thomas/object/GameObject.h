#pragma once
#include "Object.h"
#include "component\Components.h"
#include <vector>
#include <memory>

namespace thomas
{
	class Scene;
	namespace object
	{
		class GameObject : public Object
		{
		private:
			
		public:
			GameObject();
			GameObject(std::string type);
			~GameObject();

			GameObject(GameObject&& move);
			GameObject& operator=(GameObject&& move);

			GameObject(const GameObject& move) = delete;
			GameObject& operator=(const GameObject& move) = delete;

			static GameObject* Find(std::string type);

			template<typename T>
			static std::vector<GameObject*> FindGameObjectsWithComponent();
			static std::vector<GameObject*> GetGameObjects();

			template<typename T>
			T* GetComponent();

			template<typename T>
			std::vector<T*> GetComponents();

			static bool Destroy(GameObject *object);
			static void Destroy();
			template<typename T>
			static T* Instantiate(component::Transform* parent, Scene* scene);
			template<typename T>
			static T* Instantiate(math::Vector3 position, math::Quaternion rotation, Scene* scene);
			template<typename T>
			static T* Instantiate(math::Vector3 position, math::Quaternion rotation, component::Transform* parent, Scene* scene);

			bool GetActive();
			void SetActive(bool active);

			bool GetDynamic();
			void SetDynamic(bool state);

			bool GetStatic();
			void SetStatic(bool state);

			bool GetSelection();
			void SetSelection(bool selected);

			void SetRenderComponent(component::Component* renderComponent);

			component::Component* GetRenderComponent();
			/* Remove the component referenced to in the pointer.
			 comp_ptr	<<	Component to remove
			 return		>>	0 if component was found and removed.
			*/
			int RemoveComponent(void * comp_ptr);
		public:
			std::vector<component::Component*> m_components;
			component::Transform* m_transform = nullptr;
			bool m_activeSelf;


		private:
			bool m_selected;
			bool m_staticSelf;
			bool m_dynamicSelf;
			component::Component* m_renderComponent;
			static std::vector<GameObject*> s_gameObjects;
		};
		

		template<typename T>
		std::vector<GameObject*> GameObject::FindGameObjectsWithComponent()
		{
			std::vector<GameObject*> gameObjectsWithComponent;
			for (UINT i = 0; i < s_gameObjects.size(); i++)
			{
				T* component = s_gameObjects[i]->GetComponent<T>();
				if (component)
					gameObjectsWithComponent.push_back(s_gameObjects[i]);
			}
			return gameObjectsWithComponent;
		}

		template<typename T>
		T* GameObject::GetComponent()
		{

			for (UINT i = 0; i < m_components.size(); i++)
			{
				T* comp = dynamic_cast<T*>(m_components[i]);
				if (comp)
					return comp;
			}
			return NULL;
		}

		template<typename T>
		std::vector<T*> GameObject::GetComponents()
		{
			std::vector<T*> components;
			for (UINT i = 0; i < m_components.size(); i++)
			{
				T* comp = dynamic_cast<T*>(m_components[i]);
				if (comp)
					components.push_back(comp);
			}
			return components;
		}

		template<typename T>
		T * GameObject::Instantiate(component::Transform * parent, Scene * scene)
		{
			T* gameObject = Object::Instantiate<T>(scene);
			s_gameObjects.push_back(gameObject);
			gameObject->m_transform = gameObject->AddComponent<component::Transform>();
			gameObject->m_transform->SetParent(parent);
			gameObject->SetActive(true);
			gameObject->Start();
			return gameObject;
		}

		template<typename T>
		T * GameObject::Instantiate(math::Vector3 position, math::Quaternion rotation, Scene * scene)
		{
			T* gameObject = Object::Instantiate<T>(scene);
			s_gameObjects.push_back(gameObject);
			gameObject->m_transform = gameObject->AddComponent<component::Transform>();
			gameObject->m_transform->SetRotation(rotation);
			gameObject->m_transform->SetPosition(position);
			gameObject->SetActive(true);
			gameObject->Start();
			return gameObject;
		}

		template<typename T>
		T * GameObject::Instantiate(math::Vector3 position, math::Quaternion rotation, component::Transform * parent, Scene * scene)
		{
			T* gameObject = Object::Instantiate<T>(scene);
			s_gameObjects.push_back(gameObject);
			gameObject->m_transform = gameObject->AddComponent<component::Transform>();
			gameObject->m_transform->SetParent(parent);
			gameObject->m_transform->SetRotation(rotation);
			gameObject->m_transform->SetPosition(position);
			gameObject->SetActive(parent->m_gameObject->GetActive());
			gameObject->Start();
			return gameObject;
		}
	}
}