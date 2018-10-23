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
			m_soundEffectInstance = Sound::CreateInstance(m_name);
		}

		AudioClip::~AudioClip()
		{
			// This must be reset before the parent to the sound effect!
			m_soundEffectInstance.reset();
		}

		void AudioClip::OnChanged()
		{
			/*m_data.release();
			m_data = std::unique_ptr<DirectX::SoundEffect>(Sound::Instance()->LoadWave(m_path));*/
		}

		SoundEffectInstance* AudioClip::GetSoundEffectInstance() const
		{
			return m_soundEffectInstance.get();
		}
	}
}
