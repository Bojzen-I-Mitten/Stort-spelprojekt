#pragma unmanaged
#include <thomas/object/component/AudioListener.h>
#pragma managed
#include "AudioListener.h"

namespace ThomasEngine
{
	AudioListener::AudioListener() : Component(new thomas::object::component::AudioListener()) {}
}