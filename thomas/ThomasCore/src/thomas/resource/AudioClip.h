#pragma once

// Fmod
#include <fmod/fmod.hpp>

// Thomas
#include "Resource.h"

namespace thomas
{
	namespace resource
	{
		class AudioClip : public Resource
		{
		public:
			AudioClip(const std::string& file);

			FMOD::Sound* GetSound();

		private:
			// Never used, but required from abstract base class...
			void OnChanged();

			FMOD::Sound* m_sound;
		};
	}
}