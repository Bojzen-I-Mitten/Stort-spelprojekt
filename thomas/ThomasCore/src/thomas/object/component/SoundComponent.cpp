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
			m_type(SoundType::Music),
			m_channel(nullptr),
			m_clip(nullptr),
			m_is3D(false),
			m_looping(false),
			m_paused(false),
			m_volume(1.f),
			m_spreadAngle(0.f),
			m_minDistance(1.f),
			m_maxDistance(100.f)
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

					// Sound mode
					if (m_is3D)
					{
						m_clip->GetSound()->setMode(FMOD_3D);
					}
					else
					{
						m_clip->GetSound()->setMode(FMOD_2D);
					}

					// Set looping options
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

			void SoundComponent::Play(resource::AudioClip* clip, float volume, bool looping)
			{
				if (clip != nullptr)
				{
					auto sound = clip->GetSound();

					// Sound mode
					if (m_is3D)
					{
						sound->setMode(FMOD_3D);
					}
					else
					{
						sound->setMode(FMOD_2D);
					}

					// Set looping options
					if (looping)
					{
						// TODO: set mode for the channel instead of the sound?
						sound->setMode(FMOD_LOOP_NORMAL);
						sound->setLoopCount(-1); // Loop repeatedly
					}
					else
					{
						sound->setMode(FMOD_LOOP_OFF);
					}

					SoundManager::GetInstance()->GetSystem()->playSound(sound, nullptr, false, &m_channel);

					if (m_channel != nullptr)
					{
						// Set channel properties
						m_channel->setVolume(volume);
					}		
				}
			}

			void SoundComponent::Stop()
			{
				if (m_channel != nullptr)
				{
					m_channel->stop();
				}
			}

			void SoundComponent::Update()
			{
				if (m_channel != nullptr && m_is3D)
				{
					// No velocity set on the object
					m_channel->set3DAttributes(&SoundManager::GetInstance()->Vector3ToFmod(m_gameObject->m_transform->GetPosition()), NULL);
					m_channel->set3DSpread(m_spreadAngle);

					if (m_minDistance < m_maxDistance)
					{
						m_channel->set3DMinMaxDistance(m_minDistance, m_maxDistance);
					}
				}
			}

			void SoundComponent::SetType(SoundType type)
			{
				m_type = type;
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

				if (m_channel != nullptr)
				{
					m_channel->setVolume(volume);
				}
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;
			}

			void SoundComponent::SetPaused(bool paused)
			{
				m_paused = paused;
			}

			void SoundComponent::SetMute(bool mute)
			{
				m_muted = mute;

				if (m_channel != nullptr)
				{
					m_channel->setMute(mute);
				}
			}

			void SoundComponent::Set3DMinDistance(float min)
			{
				m_minDistance = min;
			}

			void SoundComponent::Set3DMaxDistance(float max)
			{
				m_maxDistance = max;
			}

			void SoundComponent::Set3DSpreadAngle(float angle)
			{
				m_spreadAngle = angle;
			}

			SoundType SoundComponent::GetType() const
			{
				return m_type;
			}

			resource::AudioClip* SoundComponent::GetClip() const
			{
				return m_clip;
			}

			float SoundComponent::GetVolume() const
			{
				return m_volume;
			}

			float SoundComponent::Get3DMinDistance() const
			{
				return m_minDistance;
			}

			float SoundComponent::Get3DMaxDistance() const
			{
				return m_maxDistance;
			}

			float SoundComponent::Get3DSpreadAngle() const
			{
				return m_spreadAngle;
			}

			bool SoundComponent::Is3D() const
			{
				return m_is3D;
			}

			bool SoundComponent::IsMute() const
			{
				return m_muted;
			}

			bool SoundComponent::IsPlaying() const
			{				
				if (m_channel != nullptr)
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
