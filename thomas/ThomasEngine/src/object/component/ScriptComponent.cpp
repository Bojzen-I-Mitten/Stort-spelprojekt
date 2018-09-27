
#pragma unmanaged
#include <thomas\object\component\ScriptComponent.h>
#pragma managed
#include "ScriptComponent.h"

namespace ThomasEngine
{
	ScriptComponent::ScriptComponent() : Component(new thomas::object::component::ScriptComponent()){}
}