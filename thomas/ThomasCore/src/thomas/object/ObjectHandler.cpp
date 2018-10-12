#include "ObjectHandler.h"
namespace thomas
{

	std::vector<object::GameObject> ObjectHandler::m_objects;
	void ObjectHandler::Init()
	{
		m_objects.resize(1000);
	}
	object::GameObject * ObjectHandler::createNewGameObject(std::string name)
	{
		m_objects.push_back(std::move(object::GameObject(name)));
		return &m_objects.back();
	}
}