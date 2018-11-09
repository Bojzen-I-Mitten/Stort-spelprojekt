#include "SoundComponent.h"

// Thomas
#include "../../resource/AudioClip.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_clip(nullptr),
			m_looping(false),
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
					SoundManager::GetInstance()->Play(m_clip->GetName());
				}
			}

			void SoundComponent::Stop()
			{
				if (m_clip != nullptr)
				{
					if (m_clip->GetSoundInfo()->channel != nullptr)
					{
						m_clip->GetSoundInfo()->channel->stop();
					}
				}
			}

			void SoundComponent::SetClip(resource::AudioClip* clip)
			{
				m_clip = clip;
			}

			void SoundComponent::SetVolume(float volume)
			{
				m_volume = volume;

				if (m_clip != nullptr)
				{
					m_clip->GetSoundInfo()->volume = m_volume;
				}
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;

				if (m_clip != nullptr)
				{
					m_clip->GetSoundInfo()->looping = m_looping;
				}
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
