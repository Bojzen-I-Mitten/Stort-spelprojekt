#pragma once
#include "GameObject.h"


namespace thomas
{
	class ObjectHandler
	{
	private:
		static std::vector<object::GameObject> m_objectsDynamic;

		static std::vector<object::GameObject> m_objectsInActive;

		static std::vector<object::GameObject> m_objectsStatic;

		static std::vector<object::GameObject> m_objectsWake;

		Scene* m_currentScene;
	public:
		static void Init();
		static object::GameObject* createNewGameObject(std::string name);
		static object::Object* setStatic(object::Object* object, object::Object*& moved);
		static object::Object* setDynamic(object::Object* object, object::Object*& moved);
	private:


	};
}