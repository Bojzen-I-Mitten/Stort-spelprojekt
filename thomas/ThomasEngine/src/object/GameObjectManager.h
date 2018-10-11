#pragma once
#pragma unmanaged 

#pragma managed


#include "GameObject.h"
using namespace System::Collections::ObjectModel;


namespace ThomasEngine {
	public ref class GameObjectManager
	{
	private:
		static List<GameObject^> m_activeObjects;
		static List<GameObject^> m_inactiveObjects;
		static array<GameObject^>^ m_staticObjects;
		static array<GameObject^>^ m_toObjectBeStarted;

		static unsigned int m_nrStatic;
	public:
		GameObjectManager();
		static GameObject^ addPrimitive(PrimitiveType type);
	private:



	};
}