#pragma unmanaged
#include <thomas\object\ObjectHandler.h>
#pragma managed
#include "GameObjectManager.h"
#include "component/RenderComponent.h"
#include "../resource/Model.h"

namespace ThomasEngine
{

	GameObjectManager::GameObjectManager()
	{
		m_staticObjects = gcnew array<GameObject^>(100);
		m_inActiveObjects = gcnew array<GameObject^>(100);
		m_activeObjects = gcnew array<GameObject^>(100);
	
	}

	GameObject^ GameObjectManager::addPrimitive(PrimitiveType type, bool isStatic)
	{

		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
	
	
		if (isStatic)
			m_staticObjects[m_nrStatic++] = gameObject;
		else
			m_activeObjects[m_nrActive++] = gameObject;
		
		return gameObject;
	}

	void GameObjectManager::isStatic(GameObject ^ gameObject)
	{
		gameObject->toBeStatic();
	}

	void GameObjectManager::createGameObjectCore(std::string name)
	{
		
	}

}
