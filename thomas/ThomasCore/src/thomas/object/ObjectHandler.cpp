#include "ObjectHandler.h"
#include "..\editor\EditorCamera.h"
#include <algorithm>

namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objectsDynamic;
	std::vector<object::GameObject> ObjectHandler::m_objectsInActive;
	std::map<size_t, std::vector<object::GameObject>> ObjectHandler::m_objectsStatic;
	std::vector<object::GameObject> ObjectHandler::m_objectsWake;

	void ObjectHandler::Init()
	{
		m_objectsDynamic.reserve(100);
		m_objectsInActive.reserve(100);
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
				UINT type = it->GetGroupID();
				if (m_objectsStatic.find(type) == m_objectsStatic.end())
				{
					m_objectsStatic.insert(std::pair<size_t, std::vector<object::GameObject> >(type, std::vector<object::GameObject>()));
					m_objectsStatic[type].reserve(1000);
				}
				m_objectsStatic[type].push_back(std::move(*it));

				moved = &m_objectsDynamic.back();

				*it = std::move(m_objectsDynamic.back());

				m_objectsDynamic.pop_back();

				m_objectsStatic[type].back().SetStatic(true);
				m_objectsStatic[type].back().SetDynamic(false);

				// Since we have to select the object to make it static, 
				// We have to set the new object as the selected one
				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic[type].back());



				return &m_objectsStatic[type].back(); // We have moved it
			}
		}

		// If we reach this branch, an error has occoured
		moved = nullptr;
		return object;
	}

	object::Object * ObjectHandler::moveStaticGroup(object::Object * object, object::Object *& moved)
	{
		UINT type = static_cast<object::GameObject*>(object)->GetGroupID();


		for (auto& it = m_objectsStatic[type].begin(); it != m_objectsStatic[type].end(); it++)
		{
			if (&(*it) == object)
			{
				UINT new_GroupID = it->GetNewGroupID();
				m_objectsStatic[new_GroupID].push_back(std::move(*it));

				moved = &m_objectsStatic[type].back();

				*it = std::move(m_objectsStatic[type].back());

				m_objectsStatic[type].pop_back();

				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic[new_GroupID].back());

				m_objectsStatic[new_GroupID].back().SetGroupID(new_GroupID);
				m_objectsStatic[new_GroupID].back().SetMoveStaticGroup(false);
				return &m_objectsStatic[new_GroupID].back(); // We have moved it
			}
		}

		moved = nullptr;
		return object;
	}

	object::Object * ObjectHandler::setDynamic(object::Object * object, object::Object *& moved)
	{
		UINT type = static_cast<object::GameObject*>(object)->GetGroupID();
		for (auto& it = m_objectsStatic[type].begin(); it != m_objectsStatic[type].end(); it++)
		{
			if (&(*it) == object)
			{
				m_objectsDynamic.push_back(std::move(*it));

				moved = &m_objectsStatic[type].back();

				*it = std::move(m_objectsStatic[type].back());

				m_objectsStatic[type].pop_back();

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