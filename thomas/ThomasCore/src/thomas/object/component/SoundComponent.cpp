#include "SoundComponent.h"

// Thomas
#include "../../SoundManager.hpp"
#include "../../resource/AudioClip.h"
#include "../GameObject.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_channel(nullptr),
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
					auto sound = m_clip->GetSound();

					// Set looping options
					// Note: if you put a sound on looping during runtime and which is playing
					// it will play separately from the same sound instance which is not set to looping
					// this behavior can then be repeated
					if (m_looping)
					{
						sound->setMode(FMOD_LOOP_NORMAL);
						sound->setLoopCount(-1); // Loop repeatedly
					}
					else
					{
						sound->setMode(FMOD_LOOP_OFF);
					}

					SoundManager::GetInstance()->GetSystem()->playSound(sound, nullptr, m_paused, &m_channel);

					if (m_channel != nullptr)
					{
						// Set channel properties
						m_channel->setVolume(m_volume);
					}
				}
			}

			void SoundComponent::Stop()
			{
				if (m_clip != nullptr && m_channel != nullptr)
				{
					m_channel->stop();
				}
			}

			void SoundComponent::Update()
			{
				if (m_clip != nullptr && m_channel != nullptr && m_is3D)
				{
					// No velocity set on the object
					m_channel->set3DAttributes(&SoundManager::GetInstance()->Vector3ToFmod(m_gameObject->m_transform->GetPosition()), NULL);
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
						m_clip->GetSound()->setMode(FMOD_3D);
					}
					else
					{
						m_clip->GetSound()->setMode(FMOD_2D);
					}
				}
			}

			void SoundComponent::SetVolume(float volume)
			{
				m_volume = volume;
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;
			}

			void SoundComponent::SetPaused(bool paused)
			{
				m_paused = paused;
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
				if (m_clip != nullptr && m_channel != nullptr)
				{
					bool playing = false;
					m_channel->isPlaying(&playing);
					
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
