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
			m_looping(true),
			m_clip(nullptr),
			m_volume(1.f)
			{
			}

			void SoundComponent::OnDisable()
			{
				Stop();
			}

			void SoundComponent::Play()
			{
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Play(m_looping);
				}
			}

			void SoundComponent::PlayOneShot()
			{
				if (m_clip != nullptr)
				{
					Sound::Play(m_clip->GetName(), m_volume);
				}
			}

			void SoundComponent::Stop()
			{
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Stop();
				}
			}

			void SoundComponent::Pause()
			{
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Pause();
				}
			}

			void SoundComponent::Resume()
			{
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Resume();
				}
			}

			void SoundComponent::SetClip(resource::AudioClip* clip)
			{
				m_clip = clip;
			}

			void SoundComponent::SetVolume(float volume)
			{
				if (volume <= 5.f && volume >= 0.f)
				{
					m_volume = volume;

					if (m_clip != nullptr)
					{
						m_clip->GetSoundEffectInstance()->SetVolume(m_volume * Sound::GetMusicVolume());
					}
				}
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;
			}

			resource::AudioClip* SoundComponent::GetClip() const
			{
				return m_clip;
			}

			float SoundComponent::GetVolume() const
			{
				return m_volume;
			}

			bool SoundComponent::IsLooping() const
			{
				return m_looping;
			}
		}
	}
}
