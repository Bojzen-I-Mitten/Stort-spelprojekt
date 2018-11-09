#pragma once
#include "GameObject.h"
#include<map>
#include "../utils/atomic/Synchronization.h"

#define LOCK_OBJECTHANDLER

namespace thomas
{
	class ObjectHandler
	{
	private:
		// Objects that have physics
		std::vector<object::GameObject> m_objectsDynamic;

		// Objects that doesn't have physcis, can be orderd into groups
		std::map<UINT, std::vector<object::GameObject>> m_objectsStatic;
#ifdef LOCK_OBJECTHANDLER
		// Access lock to prevent async. resource load issues
		utils::atomics::SpinLock m_lock;
#endif

	public:
		ObjectHandler();
		~ObjectHandler();
		void ClearAll();

		const std::vector<object::GameObject>& GetDynamicObjectVector();

		// if invalid ID is given, returns nullptr
		const std::vector<object::GameObject>* GetVectorGroup(UINT GroupID);

		object::GameObject* createNewGameObject(std::string name);
		object::Object* setStatic(object::Object* object, object::Object*& moved);
		object::Object* moveStaticGroup(object::Object* object, object::Object*& moved);
		object::Object* setDynamic(object::Object* object, object::Object*& moved);

		static ObjectHandler& Instance();

	private:

	};
}