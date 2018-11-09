#pragma once

// Thomas
#include "Resource.h"
#include "../SoundManager.hpp"

namespace thomas
{
	namespace resource
	{
		class AudioClip : public Resource
		{
		public:
			AudioClip(const std::string& file);

			SoundManager::Info* GetSoundInfo();

		private:
			// Never used, but required from abstract base class...
			void OnChanged();

			SoundManager::Info* m_soundInfo;
		};
	}
}