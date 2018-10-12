#pragma once
#include "GameObject.h"


namespace thomas
{
	class ObjectHandler
	{
	private:
		static std::vector<object::GameObject> m_objects;

	public:
		static void Init();
		static object::GameObject* createNewGameObject(std::string name);
	private:


	};
}