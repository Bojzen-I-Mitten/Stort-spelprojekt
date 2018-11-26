#include "SoundComponent.h"

// Thomas
#include "../../SoundManager.hpp"
#include "../../resource/AudioClip.h"
#include "../GameObject.h"
#include "AudioListener.h"

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
					SoundManager::GetInstance()->GetSystem()->playSound(m_clip->GetSound(), nullptr, m_paused, &m_channel);

					if (m_channel != nullptr)
					{
						// Set channel properties
						AdjustVolumeType(m_volume);

						// Sound mode
						if (m_is3D)
						{
							m_channel->setMode(FMOD_3D);
						}
						else
						{
							m_channel->setMode(FMOD_2D);
						}

						// Looping options
						if (m_looping)
						{
							m_channel->setMode(FMOD_LOOP_NORMAL);
							m_channel->setLoopCount(-1); // Loop repeatedly
						}
						else
						{
							m_channel->setMode(FMOD_LOOP_OFF);
						}
					}	
				}
			}

			void SoundComponent::Play(resource::AudioClip* clip, float volume, bool looping, bool is3D)
			{
				if (clip != nullptr)
				{
					SoundManager::GetInstance()->GetSystem()->playSound(clip->GetSound(), nullptr, false, &m_channel);

					if (m_channel != nullptr)
					{
						// Set channel properties
						AdjustVolumeType(volume);

						// Sound mode
						if (is3D)
						{
							m_channel->setMode(FMOD_3D);
						}
						else
						{
							m_channel->setMode(FMOD_2D);
						}

						// Looping options
						if (looping)
						{
							m_channel->setMode(FMOD_LOOP_NORMAL);
							m_channel->setLoopCount(-1); // Loop repeatedly
						}
						else
						{
							m_channel->setMode(FMOD_LOOP_OFF);
						}
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
					m_channel->set3DAttributes(&SoundManager::GetInstance()->Vector3ToFmod(m_gameObject->GetTransform()->GetPosition()), NULL);
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

				if (m_channel != nullptr)
				{
					if (m_is3D)
					{
						m_channel->setMode(FMOD_3D);
					}
					else
					{
						m_channel->setMode(FMOD_2D);
					}
				}
			}

			void SoundComponent::SetVolume(float volume)
			{
				m_volume = volume;

				if (m_channel != nullptr)
				{
					AdjustVolumeType(m_volume);
				}
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;

				if (m_channel != nullptr)
				{
					if (m_looping)
					{
						m_channel->setMode(FMOD_LOOP_NORMAL);
						m_channel->setLoopCount(-1); // Loop repeatedly
					}
					else
					{
						m_channel->setMode(FMOD_LOOP_OFF);
					}
				}
			}

			void SoundComponent::SetPaused(bool paused)
			{
				m_paused = paused;

				if (m_channel != nullptr)
				{
					m_channel->setPaused(paused);
				}
			}

			void SoundComponent::SetMute(bool mute)
			{
				m_muted = mute;

				if (m_channel != nullptr)
				{
					m_channel->setMute(m_muted);
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

			void SoundComponent::AdjustVolumeType(float volume)
			{
				auto listener = AudioListener::GetInstance();
				auto masterVolume = listener->GetMasterVolume();
				auto musicVolume = listener->GetMusicVolume();
				auto fxVolume = listener->GetFXVolume();
				auto voiceVolume = listener->GetVoiceVolume();

				switch (m_type)
				{
				case SoundType::Music:
					m_channel->setVolume(masterVolume * musicVolume * volume);
					break;
				case SoundType::Effect:
					m_channel->setVolume(masterVolume * fxVolume * volume);
					break;
				case SoundType::Voice:
					m_channel->setVolume(masterVolume * voiceVolume * volume);
					break;
				default:
					break;
				}
			}
		}
	}
}
