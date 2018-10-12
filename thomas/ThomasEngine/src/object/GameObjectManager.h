#pragma once
#pragma unmanaged 

#pragma managed


#include "GameObject.h"
using namespace System::Collections::ObjectModel;


namespace ThomasEngine {
	public ref class GameObjectManager
	{
	private:
		static array<GameObject^>^ m_activeObjects;
		static array<GameObject^>^ m_inActiveObjects;
		static array<GameObject^>^ m_staticObjects;
		static array<GameObject^>^ m_toObjectBeStarted;

		static unsigned int m_nrStatic;
		static unsigned int m_nrActive;

	public:
		GameObjectManager();
		static GameObject^ addPrimitive(PrimitiveType type, bool isStatic);
		static void isStatic(GameObject^ gameObject);
		static void createGameObjectCore(std::string name);
	private:



	};
}