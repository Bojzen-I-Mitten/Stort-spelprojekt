#include "SoundComponent.h"
#include "../../Sound.h"
#include "../../resource/AudioClip.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_name(""),
			m_volume(1),
			m_looping(true)
			{
			}

			void SoundComponent::Play()
			{
				// Is the instance removed after the first play?
				// Instance is never set
				if (m_instance)
				{
					m_instance->Play(m_looping);
				}
			}

			void SoundComponent::PlayOneShot(std::string name, float volume)
			{
				return Sound::Instance()->Play(name, volume);
			}

			void SoundComponent::Pause()
			{
				if (m_instance)
				{
					m_instance->Pause();
				}
			}

			void SoundComponent::Resume()
			{
				if (m_instance)
				{
					m_instance->Resume();
				}
			}

			void SoundComponent::SetClip(resource::AudioClip* clip)
			{
				m_instance = clip->CreateInstance();

				if (m_instance)
				{
					m_instance->SetVolume(m_volume);
				}
				else
				{
					//Sound::LoadWaveBank(name);
					//Sound::LoadWave(name);

					//m_instance = Sound::CreateInstance(name);
					//m_instance->SetVolume(m_volume);
					//m_name = name;
					//return true;
					//return false;
				}
			}

			resource::AudioClip* SoundComponent::GetClip()
			{
				return m_clip;
			}

			void SoundComponent::SetVolume(float volume)
			{
				if (volume > 5)
					return false;

				m_volume = volume;
				if (m_instance)
				{
					m_instance->SetVolume(volume * Sound::Instance()->GetMusicVolume());
					
				}
				return true;
			}

			float SoundComponent::GetVolume()
			{
				return m_volume;
			}

			void SoundComponent::SetLooping(bool loop)
			{
				m_looping = loop;
			}

			bool SoundComponent::IsLooping()
			{
				return m_looping;
			}
		}
	}
}
