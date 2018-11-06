#include "ObjectHandler.h"
#include "..\editor\EditorCamera.h"
#include <algorithm>

namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objectsDynamic;

	std::map<UINT, std::vector<object::GameObject>> ObjectHandler::m_objectsStatic;

	void ObjectHandler::Init()
	{
		m_objectsDynamic.reserve(1000);
	}

	void ObjectHandler::ClearAll()
	{
		// Clean all the arrays, used when scene is emptied so that we don't get any
		// Objects without pointers
		//for (auto& key : m_objectsStatic)
		//{
		//	key.second.clear();
		//}

		//m_objectsDynamic.clear();
	}

	std::vector<object::GameObject>* ObjectHandler::GetDynamicObjectVector()
	{
		return &m_objectsDynamic;
	}

	std::vector<object::GameObject>* ObjectHandler::GetVectorGroup(UINT GroupID)
	{
		if (m_objectsStatic.find(GroupID) != m_objectsStatic.end())
		{
			return &m_objectsStatic[GroupID];
		}
		return nullptr;
	}

	object::GameObject * ObjectHandler::createNewGameObject(std::string name)
	{
		m_objectsDynamic.push_back(std::move(object::GameObject(name)));
		return &m_objectsDynamic.back();
	}

	object::Object* ObjectHandler::setStatic(object::Object* object, object::Object*& moved)
	{
		int i = 0;
		for (auto& it = m_objectsDynamic.begin(); it != m_objectsDynamic.end(); it++)
		{
			if (&(*it) == object)
			{
				UINT type = it->GetGroupID();
				if (m_objectsStatic.find(type) == m_objectsStatic.end())
				{
					m_objectsStatic.insert(std::pair<UINT, std::vector<object::GameObject> >(type, std::vector<object::GameObject>()));
					m_objectsStatic[type].reserve(1000);
				}

				// Move the object to it's new location
				object::GameObject *ptr = it._Ptr;
				m_objectsStatic[type].push_back(std::move(*ptr));

				// Fetch the old native pointer adress, this will be a dangeling pointer
				// But it will be used to update the new adress of this object
				moved = &m_objectsDynamic.back();

				// Move the object at the back, to remove any holes in the array
				if (ptr != &m_objectsDynamic.back())
					*ptr = std::move(m_objectsDynamic.back());
				// remove the now empty object at the back
				m_objectsDynamic.pop_back();

				// update the states of the object
				m_objectsStatic[type].back().SetStatic();

				// Since we have to select the object to make it static, 
				// We have to set the new object as the selected one
				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic[type].back());


				// this is the new adress of the object
				return &m_objectsStatic[type].back(); // We have moved it
			}
			i++;
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

				// If new key, allocate memory to avoid loss of objects when reallocation happens
				if (m_objectsStatic.find(new_GroupID) == m_objectsStatic.end())
				{
					m_objectsStatic.insert(std::pair<UINT, std::vector<object::GameObject> >(new_GroupID, std::vector<object::GameObject>()));
					m_objectsStatic[new_GroupID].reserve(1000);
				}

				m_objectsStatic[new_GroupID].push_back(std::move(*it));

				moved = &m_objectsStatic[type].back();

				*it = std::move(m_objectsStatic[type].back());

				m_objectsStatic[type].pop_back();

				editor::EditorCamera::Instance()->SelectObject(&m_objectsStatic[new_GroupID].back());

				m_objectsStatic[new_GroupID].back().SetGroupID(new_GroupID);
				m_objectsStatic[new_GroupID].back().SetMoveStaticGroup(false);
				m_objectsStatic[new_GroupID].back().SetStatic();
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

				m_objectsDynamic.back().SetDynamic();

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