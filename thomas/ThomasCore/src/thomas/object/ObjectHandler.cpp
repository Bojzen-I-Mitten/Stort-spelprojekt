#include "ObjectHandler.h"
#include "..\editor\EditorCamera.h"
#include <algorithm>
namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objectsDynamic;
	std::vector<object::GameObject> ObjectHandler::m_objectsInActive;
	std::vector<object::GameObject> ObjectHandler::m_objectsStatic;
	std::vector<object::GameObject> ObjectHandler::m_objectsWake;

	void ObjectHandler::Init()
	{
		m_objectsDynamic.reserve(100);
		m_objectsInActive.reserve(100);
		m_objectsStatic.reserve(100);
		m_objectsWake.reserve(100);
	}

	object::GameObject * ObjectHandler::createNewGameObject(std::string name)
	{
		m_objectsDynamic.push_back(std::move(object::GameObject(name)));
		return &m_objectsDynamic.back();
	}

	object::Object* ObjectHandler::setStatic(object::Object* object, object::Object*& moved)
	{

		for (auto& it = m_objectsDynamic.begin(); it != m_objectsDynamic.end(); it++)
		{
			if (&(*it) == object)
			{
				
				m_objectsStatic.push_back(std::move(*it));

				moved = &m_objectsDynamic.back();

				*it = std::move(m_objectsDynamic.back());

				m_objectsDynamic.pop_back();

				m_objectsStatic.back().SetStatic(true);
				m_objectsStatic.back().SetDynamic(false);

				// Since we have to select the object to make it static, 
				// We have to set the new object as the selected one
				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic.back());

				return &m_objectsStatic.back(); // We have moved it
			}
		}

		// If we reach this branch, an error has occoured
		moved = nullptr;
		return object;
	}

	object::Object * ObjectHandler::setDynamic(object::Object * object, object::Object *& moved)
	{
		for (auto& it = m_objectsStatic.begin(); it != m_objectsStatic.end(); it++)
		{
			if (&(*it) == object)
			{
				m_objectsDynamic.push_back(std::move(*it));

				moved = &m_objectsStatic.back();

				*it = std::move(m_objectsStatic.back());

				m_objectsStatic.pop_back();

				m_objectsDynamic.back().SetStatic(false);
				m_objectsDynamic.back().SetDynamic(true);

				// Since we have to select the object to make it static, 
				// We have to set the new object as the selected one
				editor::EditorCamera::Instance()->SelectObject(&m_objectsDynamic.back());

				return &m_objectsDynamic.back(); // We have moved it
			}
		}

		// If we reach this branch, an error has occoured
		moved = nullptr;
		return object;
	}

	
}