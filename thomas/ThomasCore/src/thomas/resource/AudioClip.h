#pragma once

#include "Resource.h"
#include <memory>

namespace DirectX
{
	class SoundEffect;
	class SoundEffectInstance;
}

using namespace DirectX;

namespace thomas
{
	namespace resource
	{
		class AudioClip : public Resource
		{
		private:
			void OnChanged(); // Never used...?

		public:
			AudioClip(const std::string& file);
			~AudioClip();

			//SoundEffect* GetSoundEffect() const;
			SoundEffectInstance* GetSoundEffectInstance() const;

		private:
			std::unique_ptr<SoundEffectInstance> m_soundEffectInstance;
		};
	}
}