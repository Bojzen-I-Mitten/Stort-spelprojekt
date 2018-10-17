#include "ObjectHandler.h"
#include "..\editor\EditorCamera.h"
#include <algorithm>
namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objectsActive;
	std::vector<object::GameObject> ObjectHandler::m_objectsInActive;
	std::vector<object::GameObject> ObjectHandler::m_objectsStatic;
	std::vector<object::GameObject> ObjectHandler::m_objectsWake;

	void ObjectHandler::Init()
	{
		m_objectsActive.reserve(100);
		m_objectsInActive.reserve(100);
		m_objectsStatic.reserve(100);
		m_objectsWake.reserve(100);
	}

	object::GameObject * ObjectHandler::createNewGameObject(std::string name)
	{
		m_objectsActive.push_back(std::move(object::GameObject(name)));
		return &m_objectsActive.back();
	}

	object::Object* ObjectHandler::setStatic(object::Object* object, object::Object*& moved)
	{

		for (auto& it = m_objectsActive.begin(); it != m_objectsActive.end(); it++)
		{
			if (&(*it) == object)
			{
				//std::iter_swap(it, m_objectsInActive.end() - 1);

				m_objectsStatic.push_back(std::move(*it));

				moved = &m_objectsActive.back();

				*it = std::move(m_objectsActive.back());

				m_objectsActive.pop_back();

				m_objectsStatic.back().SetStatic();

				// Since we have to select the object to make it static, 
				// We have to set the new object as the selected one
				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic.back());

				return &m_objectsStatic.back(); // We have moved it


			}
		}

		return object;
	}

	object::GameObject * ObjectHandler::setActive(object::GameObject * object)
	{
		for (auto& it = m_objectsStatic.begin(); it != m_objectsStatic.end(); it++)
		{
			if (&(*it) == object)
			{
				//std::iter_swap(it, m_objectsInActive.end() - 1);

				m_objectsActive.push_back(std::move(*it));

				*it = std::move(m_objectsActive.back());

				m_objectsStatic.pop_back();

				m_objectsStatic.back().SetStatic();
				m_objectsStatic.back().SetActive(true);

				return &m_objectsStatic.back(); // We have moved it
			}
		}

		return object;
	}
	
}