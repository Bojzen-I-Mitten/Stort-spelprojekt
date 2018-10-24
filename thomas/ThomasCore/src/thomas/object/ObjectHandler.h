#pragma once
#include "GameObject.h"
#include<map>

namespace thomas
{
	class ObjectHandler
	{
	private:
		static std::vector<object::GameObject> m_objectsDynamic;

		static std::vector<object::GameObject> m_objectsInActive;

		static std::map<size_t, std::vector<object::GameObject>> m_objectsStatic;

	public:
		static void Init();
		static void ClearAll();
		static object::GameObject* createNewGameObject(std::string name);
		static object::Object* setStatic(object::Object* object, object::Object*& moved);
		static object::Object* moveStaticGroup(object::Object* object, object::Object*& moved);
		static object::Object* setDynamic(object::Object* object, object::Object*& moved);

	private:


	};
}