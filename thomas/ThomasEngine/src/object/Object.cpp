#include "Object.h"
#include "../Scene.h"
#include "Component.h"
#include "GameObject.h"
namespace ThomasEngine
{
	List<Object^>^ Object::GetObjectsOfType(Type ^ type)
	{
		List<Object^>^ list = gcnew List<Object^>();
		for (int i = 0; i < s_objects.Count; i++) {
			if (s_objects[i]->GetType() == type) {
				if ((Component::typeid)->IsAssignableFrom(type)) {
					ThomasEngine::Component^ component = (Component^)s_objects[i];
					if (component->gameObject->inScene)
						list->Add((Object^)component);
				}
				else {
					list->Add((Object^)s_objects[i]);
				}
			}

		}
		return list;
	}
}