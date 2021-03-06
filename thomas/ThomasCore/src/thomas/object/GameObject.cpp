#include "GameObject.h"
#include "../editor/EditorCamera.h"
#include "../ThomasCore.h"
#include "../Common.h"
namespace thomas
{
	namespace object
	{
		std::vector<GameObject*> GameObject::s_gameObjects;
		GameObject::GameObject()
		{
			m_activeSelf = true;
			m_name = "VoidName";
			m_selected = false;
			m_static = false;
			m_GroupID = 0;
			new_GroupID = 0;
			m_layer = 0;
			m_moveStaticGroup = false;
		}

		GameObject::GameObject(std::string name)
		{
			m_activeSelf = true;
			m_name = name;
			m_selected = false;
			m_static = false;
			m_GroupID = 0;
			new_GroupID = 0;
			m_layer = 0;
			m_moveStaticGroup = false;
		}

		GameObject::~GameObject()
		{
			this->m_components.clear();
#ifdef _EDITOR
			editor::EditorCamera::Instance()->UnselectObject(this);
#endif
			Object::Destroy();
			this->m_components.clear();
		}

		GameObject::GameObject(GameObject && move)
			: m_components(std::move(move.m_components)), 
			m_transform(move.m_transform)
		{
			m_selected = std::move(move.m_selected);
			m_guid = std::move(move.m_guid);
			m_name = std::move(move.m_name);



			m_transform = move.m_transform;
			move.m_transform = nullptr;
			m_activeSelf = move.m_activeSelf;
			m_static = move.m_static;
			m_selected = move.m_selected;
			m_moveStaticGroup = move.m_moveStaticGroup;
			m_GroupID = move.m_GroupID;
			new_GroupID = move.new_GroupID;
			
			//object::Object::Add(this);
			for (auto it : m_components)
			{
				it->m_gameObject = this;
			}
		}


		GameObject& GameObject::operator=(GameObject && move)
		{
			if (this == &move)
				return *this;

			m_components = std::move(move.m_components);
			m_selected = std::move(move.m_selected);
			m_guid = std::move(move.m_guid);
			m_name = std::move(move.m_name);



			m_transform = move.m_transform;
			move.m_transform = nullptr;

			m_activeSelf = move.m_activeSelf;
			m_static = move.m_static;
			m_selected = move.m_selected;
			m_moveStaticGroup = move.m_moveStaticGroup;
			m_GroupID = move.m_GroupID;
			new_GroupID = move.new_GroupID;


			//object::Object::Add(this);
			for (auto it : m_components)
			{
				it->m_gameObject = this;
			}
			return *this;
		}

		GameObject * GameObject::Find(std::string type)
		{
			for (unsigned int i = 0; i < s_gameObjects.size(); i++)
			{
				if (s_gameObjects[i]->m_name == type)
					return s_gameObjects[i];
			}
			return NULL;
		}

		std::vector<GameObject*> GameObject::GetGameObjects()
		{
			return s_gameObjects;
		}

		bool GameObject::Destroy(GameObject * gameObject)
		{
			//for (unsigned int i = 0; i < gameObject->m_components.size(); i++)
			//{
			//	if (gameObject->m_components[i])
			//	{
			//		Object::Destroy(gameObject->m_components[i]);
			//		
			//	}

			//}
			//gameObject->m_components.clear();

			//for (unsigned int i = 0; i < s_gameObjects.size(); i++)
			//{
			//	if (s_gameObjects[i] == gameObject)
			//	{
			//		s_gameObjects.erase(s_gameObjects.begin() + i);

			//	}
			//}

			//Object::Destroy(gameObject);

			return true;
		}
		/* Remove the component referenced to in the pointer.
		 comp_ptr	<<	Component to remove
		 return		>>	0 if component was found and removed.
		*/
		int GameObject::RemoveComponent(void * comp_ptr)
		{
			for (int i = 0; i < m_components.size(); i++)
			{
				if (m_components[i] == comp_ptr)
				{
					// Swap out element at end.
					std::swap(m_components[i], m_components.back());
					m_components.pop_back();
					return 0;
				}
			}
			return 1;
		}

		component::Transform * GameObject::GetTransform()
		{
			return m_transform;
		}

		const std::vector<component::Component*>& GameObject::getComponents()
		{
			return m_components;
		}

		void GameObject::SetTransform(component::Transform * t)
		{
			m_transform = t;
		}

		void GameObject::Destroy()
		{
			for (unsigned int i = 0; i < s_gameObjects.size(); ++i)
			{
				Destroy(s_gameObjects[i]);
			}
			s_gameObjects.clear();
		}

		bool GameObject::GetActive()
		{
			if (m_transform && m_transform->GetParent())
			{
				return m_activeSelf && m_transform->GetParent()->m_gameObject->GetActive();
			}
			return m_activeSelf;
		}

		void GameObject::SetActive(bool active)
		{
			m_activeSelf = active;
			/*
			if (m_transform && m_transform->GetParent())
			{
				return m_transform->GetParent()->m_gameObject->SetActive(active);
			}
			*/
		}

		int GameObject::GetLayer()
		{
			return m_layer;
		}
		void GameObject::SetLayer(int layer)
		{
			m_layer = layer;
		}

		void GameObject::SetSelection(bool selected)
		{
			m_selected = selected;
		}

		bool GameObject::ChangeGroupID(UINT id)
		{
			if (m_static && id != m_GroupID)
			{
				new_GroupID = id;
				m_moveStaticGroup = true;
				return true;
			}
			return false;
		}

		void GameObject::SetMoveStaticGroup(bool state)
		{
			m_moveStaticGroup = state;
		}

		bool GameObject::GetMoveStaticGroup()
		{
			return m_moveStaticGroup;
		}

		void GameObject::appendComponent(component::Component * comp)
		{
			m_components.push_back(comp);
		}

		UINT GameObject::GetNewGroupID()
		{
			return new_GroupID;
		}

		UINT GameObject::GetGroupID()
		{
			return m_GroupID;
		}

		void GameObject::SetGroupID(UINT id)
		{
			m_GroupID = id;
		}


		bool GameObject::GetStatic()
		{
			return m_static;
		}

		bool GameObject::GetDynamic()
		{
			return !m_static;
		}

		void GameObject::SetDynamic()
		{
			m_static = false;
		}

		void GameObject::SetStatic()
		{
			m_static = true;
		}

		bool GameObject::GetSelection()
		{
			return m_selected;
		}

	}
}