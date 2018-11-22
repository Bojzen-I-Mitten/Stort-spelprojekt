#include "ScriptingUtility.h"
#include "../Scene.h"
#include "../ThomasManaged.h"


namespace ThomasEngine
{
	namespace Script
	{
		generic<typename T>
			where T : Component
		inline IEnumerable<T>^ ScriptUtility::GetComponentsOfType()
		{
			return ThomasWrapper::CurrentScene->getComponentsOfType<T>();
		}
		generic<typename T>
			where T : Component
		T ScriptUtility::FindComponent()
		{
			return ThomasWrapper::CurrentScene->findFirstComponent<T>();
		}
	}
}