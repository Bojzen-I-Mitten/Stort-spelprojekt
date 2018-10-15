#include "GameObject.h"
#include "../editor/EditorCamera.h"
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
		}

		GameObject::GameObject(std::string name)
		{
			m_activeSelf = true;
			m_name = name;
			m_selected = false;
			m_staticSelf = false;
		}

		GameObject::~GameObject()
		{
			Object::Destroy(this);
#ifdef _EDITOR
			editor::EditorCamera::Instance()->UnselectObject(this);
#endif
			this->m_components.clear();
		}

		//GameObject::GameObject(GameObject && move)
		//	: m_components(std::move(move.m_components)), 
		//	m_transform(std::move(move.m_transform))
		//{
		//	m_activeSelf = move.m_activeSelf;
		//	m_staticSelf = move.m_staticSelf;
		//
		//	move.m_transform = nullptr;
		//	m_selected = move.m_selected;
		//}


		//GameObject& GameObject::operator=(GameObject && move)
		//{
		//	if (this != &move)
		//	{
		//		m_components = std::move(move.m_components);
		//		m_transform = std::move(m_transform);
		//		m_activeSelf = move.m_activeSelf;
		//		m_staticSelf = move.m_staticSelf;

		//		move.m_transform = nullptr;
		//		m_selected = move.m_selected;
		//	}
		//	return *this;
		//}

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
				return m_activeSelf && m_transform->GetParent()->m_gameObject->m_activeSelf;
			}
			return m_activeSelf;
		}

		void GameObject::SetActive(bool active)
		{
			m_activeSelf = active;
			if (m_transform && m_transform->GetParent())
			{
				return m_transform->GetParent()->m_gameObject->SetActive(active);
			}
		}

		void GameObject::SetSelection(bool selected)
		{
			m_selected = selected;
		}

		void GameObject::SetStatic()
		{
			m_staticSelf = !m_staticSelf;
		}

		bool GameObject::GetStatic()
		{
			return m_staticSelf;
		}

		bool GameObject::GetSelection()
		{
			return m_selected;
		}
	}
}