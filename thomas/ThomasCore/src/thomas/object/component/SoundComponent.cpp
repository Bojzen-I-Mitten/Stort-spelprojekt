#include "SoundComponent.h"

#include "AudioListener.h"
#include "../../Sound.h"
#include "../../resource/AudioClip.h"
#include "../../ThomasTime.h"
#include "Transform.h"
#include "../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_looping(true),
			m_clip(nullptr),
			m_volume(0.5f),
			m_volumeFactor(1.f),
			m_is3D(false)
			{
				m_emitter = new DirectX::AudioEmitter();
			}

			void SoundComponent::OnDisable()
			{
				Stop(); // Would be better to just use the sound engine to stop when not in play mode and then resume...
			}

			void SoundComponent::Update()
			{
				if (m_is3D && AudioListener::GetInstance())
				{
					m_emitter->Update(m_gameObject->m_transform->GetPosition(), m_gameObject->m_transform->Up(), ThomasTime::GetActualDeltaTime());
					//Apply3D(AudioListener::GetInstance()->m_gameObject->m_transform->GetPosition(), m_gameObject->m_transform->GetPosition());
					if (m_clip != nullptr)
					{
						m_clip->GetSoundEffectInstance()->Apply3D(AudioListener::GetInstance()->GetListner(), *m_emitter);
					}
				}
			}

			void SoundComponent::Apply3D(const Vector3& listenerPos, const Vector3& sourcePos)
			{
				// Apply 3D-effect with attenuation formula based on Inverse Square Law
				if (m_clip != nullptr)
				{
		
					
					float attenuation = Sound::VolumeTodB(m_volume * m_volumeFactor) - Sound::VolumeTodB((sourcePos - listenerPos).Length());

					
					if (attenuation > 5.0f)
					{
					attenuation = 5.0f;
					}
					else if (attenuation < 0.0f)
					{
						attenuation = 0.0f;
					}
					m_clip->GetSoundEffectInstance()->SetVolume(Sound::dbToVolume(attenuation));
				
				}
			}

			void SoundComponent::Play()
			{
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Play(m_looping);
					SetVolume(m_volume);
				}
			}

			void SoundComponent::PlayOneShot()
			{
				if (m_clip != nullptr)
				{
					m_volume;
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

			bool SoundComponent::IsPlaying() const
			{
				if (m_clip != nullptr)
				{
					return Sound::IsPlaying(m_clip->GetName());	
				}

				return false;
			}

			bool SoundComponent::IsPaused() const
			{
				if (m_clip != nullptr)
				{
					return Sound::IsPaused(m_clip->GetName());
				}

				return false;
			}

			bool SoundComponent::HasStopped() const
			{
				if (m_clip != nullptr)
				{
					return Sound::HasStopped(m_clip->GetName());
				}

				return false;
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
						m_clip->GetSoundEffectInstance()->SetVolume(Sound::dbToVolume(m_volume * m_volumeFactor));
					}
				}
			}

			void SoundComponent::SetVolumeFactor(float volumeFactor)
			{
				if (volumeFactor <= 1.f && volumeFactor >= 0.f)
				{
					m_volumeFactor = volumeFactor;
				}
			}

			void SoundComponent::SetLooping(bool looping)
			{
				m_looping = looping;
			}

			void SoundComponent::Set3D(bool value)
			{
				m_is3D = value;
			}

			resource::AudioClip* SoundComponent::GetClip() const
			{
				return m_clip;
			}

			float SoundComponent::GetVolume() const
			{
				return m_volume;
			}

			float SoundComponent::GetVolumeFactor() const
			{
				return m_volumeFactor;
			}

			bool SoundComponent::IsLooping() const
			{
				return m_looping;
			}
			bool SoundComponent::is3D() const
			{
				return m_is3D;
			}
		}
	}
}
