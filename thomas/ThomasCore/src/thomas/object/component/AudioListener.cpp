#include "AudioListener.h"
#include "../../Common.h"
#include "../../ThomasCore.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			AudioListener* AudioListener::s_instance = nullptr;
			float AudioListener::s_masterVolume = 1.f;
			float AudioListener::s_musicVolume = 1.f;
			float AudioListener::s_fxVolume = 1.f;
			float AudioListener::s_voiceVolume = 1.f;

			void AudioListener::OnEnable()
			{
				if (s_instance)
					LOG("Cannot have multiple AudioListeners in a scene");
				s_instance = this;
			}

			void AudioListener::OnDisable()
			{
				s_instance = nullptr;
			}

			void AudioListener::SetMasterVolume(float volume)
			{
				s_masterVolume = volume;
			}

			void AudioListener::SetMusicVolume(float volume)
			{
				s_musicVolume = volume;
			}

			void AudioListener::SetFXVolume(float volume)
			{
				s_fxVolume = volume;
			}

			void AudioListener::SetVoiceVolume(float volume)
			{
				s_voiceVolume = volume;
			}

			float AudioListener::GetMasterVolume() const
			{
				return s_masterVolume;
			}

			float AudioListener::GetMusicVolume() const
			{
				return s_musicVolume;
			}

			float AudioListener::GetFXVolume() const
			{
				return s_fxVolume;
			}

			float AudioListener::GetVoiceVolume() const
			{
				return s_voiceVolume;
			}

			AudioListener* AudioListener::GetInstance()
			{
				return s_instance;
			}
		}
	}
}

