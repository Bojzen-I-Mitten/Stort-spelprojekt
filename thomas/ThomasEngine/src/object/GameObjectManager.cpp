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
	
		m_activeObjects[m_nrActive++] = gameObject;
		return gameObject;
	}

	void GameObjectManager::createGameObjectCore(std::string name)
	{
		
	}

	void GameObjectManager::makeStatic(GameObject^ object)
	{
		if (!object->staticSelf::get())
			object->staticSelf::set(true);
	}

	void GameObjectManager::RemoveMarkedObjects()
	{
		for (int i = 0; i < m_nrActive; i++)
		{
			GameObject^ temp_object = m_activeObjects[i];
			if (temp_object->m_isDestroyed)
			{
				temp_object->Destroy();
			}
		}
	}
}
