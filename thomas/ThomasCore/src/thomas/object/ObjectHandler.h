#pragma once
#include "GameObject.h"


namespace thomas
{
	class ObjectHandler
	{
	private:
		static std::vector<object::GameObject> m_objectsActive;

		static std::vector<object::GameObject> m_objectsInActive;

		static std::vector<object::GameObject> m_objectsStatic;

		static std::vector<object::GameObject> m_objectsWake;

		Scene* m_currentScene;
	public:
		static void Init();
		static object::GameObject* createNewGameObject(std::string name);
		static object::GameObject* setStatic(object::GameObject* object);
		static object::GameObject* setActive(object::GameObject* object);
	private:


	};
}