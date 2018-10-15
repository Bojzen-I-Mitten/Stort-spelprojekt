#include "ObjectHandler.h"
#include <algorithm>
namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objectsActive;
	std::vector<object::GameObject> ObjectHandler::m_objectsInActive;
	std::vector<object::GameObject> ObjectHandler::m_objectsStatic;
	std::vector<object::GameObject> ObjectHandler::m_objectsWake;

	void ObjectHandler::Init()
	{

	}

	object::GameObject * ObjectHandler::createNewGameObject(std::string name)
	{
		m_objectsActive.push_back(std::move(object::GameObject(name)));
		return &m_objectsActive.back();
	}

	object::GameObject* ObjectHandler::setStatic(object::GameObject* object)
	{
		for (auto& it = m_objectsActive.begin(); it != m_objectsActive.end(); it++)
		{
			if (&(*it) == object)
			{
				//std::iter_swap(it, m_objectsInActive.end() - 1);

				m_objectsStatic.push_back(std::move(*it));

				*it = std::move(m_objectsActive.back());

				m_objectsActive.pop_back();

				m_objectsStatic.back().SetStatic();

				return &m_objectsStatic.back(); // We have moved it
			}
		}
	}
}