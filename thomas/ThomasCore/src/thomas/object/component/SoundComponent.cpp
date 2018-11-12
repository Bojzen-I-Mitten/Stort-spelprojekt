#include "SoundComponent.h"

// Thomas
#include "../../resource/AudioClip.h"
#include "../GameObject.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_clip(nullptr),
			m_is3D(false),
			m_looping(false),
			m_paused(false),
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

			void SoundComponent::Update()
			{
				if (m_clip != nullptr && m_is3D)
				{
					// No velocity set on the object
					m_clip->GetSoundInfo()->channel->set3DAttributes(&SoundManager::GetInstance()->Vector3ToFmod(m_gameObject->m_transform->GetPosition()), 
						NULL);
				}
			}

			void SoundComponent::SetClip(resource::AudioClip* clip)
			{
				m_clip = clip;
			}

			void SoundComponent::Set3D(bool is3D)
			{
				m_is3D = is3D;

				if (m_clip != nullptr)
				{
					if (m_is3D)
					{
						m_clip->GetSoundInfo()->sound->setMode(FMOD_3D);
					}
					else
					{
						m_clip->GetSoundInfo()->sound->setMode(FMOD_2D);
					}
				}
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

			void SoundComponent::SetPaused(bool paused)
			{
				m_paused = paused;

				if (m_clip != nullptr)
				{
					m_clip->GetSoundInfo()->paused = m_paused;
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

			bool SoundComponent::Is3D() const
			{
				return m_is3D;
			}

			bool SoundComponent::IsPlaying() const
			{				
				if (m_clip != nullptr)
				{
					bool playing = false;
					m_clip->GetSoundInfo()->channel->isPlaying(&playing);
					
					return playing;
				}

				return false;
			}

			bool SoundComponent::IsLooping() const
			{
				return m_looping;
			}

			bool SoundComponent::IsPaused() const
			{
				return m_paused;
			}
		}
	}
}
