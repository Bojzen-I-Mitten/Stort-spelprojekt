#include "AudioClip.h"

// Thomas
#include "../SoundManager.hpp"

namespace thomas
{
	namespace resource
	{
		AudioClip::AudioClip(const std::string& file) : 
		Resource(file)
		{
			// TODO: is there a way to allow users to set streaming functionality for clips later on?
			SoundManager::GetInstance()->LoadSound(m_name, m_path);
			m_sound = SoundManager::GetInstance()->GetSound(m_name);
		}

		FMOD::Sound* AudioClip::GetSound()
		{
			return m_sound;
		}

		void AudioClip::OnChanged()
		{
		}
		std::unique_ptr<SoundEffectInstance> AudioClip::CreateSoundEffectInstance()
		{
			return Sound::GetSoundInfo(m_name).soundEffect->CreateInstance(SoundEffectInstance_Use3D);
		}
	}
}
