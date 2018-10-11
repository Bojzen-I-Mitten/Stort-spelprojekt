#include "GameObjectManager.h"
#include "component/RenderComponent.h"
#include "../resource/Model.h"

namespace ThomasEngine
{

	GameObjectManager::GameObjectManager()
	{
		m_staticObjects = gcnew array<GameObject^>(100);
	}

	GameObject^ GameObjectManager::addPrimitive(PrimitiveType type)
	{
		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
	
	
		m_staticObjects[m_nrStatic++] = gameObject;
		return gameObject;
	}

}
