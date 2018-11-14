#include "AudioClip.h"
#include "../Sound.h"

namespace thomas
{
	namespace resource
	{
		AudioClip::AudioClip(const std::string& file) : 
		Resource(file)
		{
			Sound::LoadSound(m_name, m_path);
			m_soundEffectInstance = Sound::GetSoundInfo(m_name).soundEffectInstance.get();
		}

		void AudioClip::OnChanged()
		{
		}

		SoundEffectInstance* AudioClip::GetSoundEffectInstance() const
		{
			return m_soundEffectInstance;
		}
		std::unique_ptr<SoundEffectInstance> AudioClip::CreateSoundEffectInstance()
		{
			return Sound::GetSoundInfo(m_name).soundEffect->CreateInstance(SoundEffectInstance_Use3D);
		}
	}
}
