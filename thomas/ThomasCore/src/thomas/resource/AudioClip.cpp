#include "AudioClip.h"

namespace thomas
{
	namespace resource
	{
		AudioClip::AudioClip(const std::string& file) : 
		Resource(file)
		{
			// TODO: is there a way to allow users to set streaming functionality for clips later on?
			SoundManager::GetInstance()->LoadSound(m_name, m_path);
			m_soundInfo = &SoundManager::GetInstance()->GetSoundInfo(m_name);
		}

		SoundManager::Info* AudioClip::GetSoundInfo()
		{
			return m_soundInfo;
		}

		void AudioClip::OnChanged()
		{
		}
	}
}
