#pragma unmanaged
#include <thomas\object\component\physics\Ragdoll.h>

#pragma managed
#include "Ragdoll.h"

#pragma managed
namespace ThomasEngine
{
	
	Ragdoll::Ragdoll() : Component(new thomas::object::component::Ragdoll()) {}

}