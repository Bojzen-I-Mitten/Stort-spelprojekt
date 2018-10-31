#pragma once

#include "Component.h"
#include "../../utils/Math.h"

namespace thomas
{
	using namespace math;

	namespace object
	{
		namespace component
		{
			// There can only be one audio listener in a scene (singleton)
			// The audio listener should have knowledge about all sound components in the scene
			class AudioListener : public Component
			{
			private:
				AudioListener();
				~AudioListener() = default;

				// We also don't want a user to be able to copy an instance of this class,
				// thus we define the copy and move construtors/assigments as private
				AudioListener(const AudioListener&) {}
				AudioListener& operator = (const AudioListener&) {}
				AudioListener(const AudioListener&&) {}
				AudioListener& operator = (const AudioListener&&) {}
				
			public:
				static AudioListener* GetInstance();

			private:
			};
		}
	}
}