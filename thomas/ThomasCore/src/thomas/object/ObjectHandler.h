#pragma once
#include "GameObject.h"
#include<map>

namespace thomas
{
	class ObjectHandler
	{
	private:
		// Objects that have physics
		static std::vector<object::GameObject> m_objectsDynamic;

		// Objects that doesn't have physcis, can be orderd into groups
		static std::map<UINT, std::vector<object::GameObject>> m_objectsStatic;

	public:
		static void Init();
		static void ClearAll();

		static std::vector<object::GameObject>* GetDynamicObjectVector();

		// if invalid ID is given, returns nullptr
		static std::vector<object::GameObject>* GetVectorGroup(UINT GroupID);

		static object::GameObject* createNewGameObject(std::string name);
		static object::Object* setStatic(object::Object* object, object::Object*& moved);
		static object::Object* moveStaticGroup(object::Object* object, object::Object*& moved);
		static object::Object* setDynamic(object::Object* object, object::Object*& moved);

	private:


	};
}