#include "SoundComponent.h"

#include "AudioListener.h"
#include "../../Sound.h"
#include "../../resource/AudioClip.h"
#include "../../ThomasTime.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../../ThomasCore.h"
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
			m_is3D(false),
			m_soundEffectInstance(nullptr)
			{
				m_emitter = new DirectX::AudioEmitter();
			}

			SoundComponent::~SoundComponent()
			{
				if (m_soundEffectInstance)
				{
					m_soundEffectInstance->Stop();
					m_soundEffectInstance.release();
				}
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
					Apply3D(AudioListener::GetInstance()->m_gameObject->m_transform->GetPosition(), m_gameObject->m_transform->GetPosition());

				}
			}

			void SoundComponent::Apply3D(const Vector3& listenerPos, const Vector3& sourcePos)
			{
				// Apply 3D-effect with attenuation formula based on Inverse Square Law
				if (m_clip != nullptr)
				{
					float range = 10.0f;
					float volumeModifier = range / Vector3::Distance(m_emitter->Position, AudioListener::GetInstance()->GetListner().Position);
					volumeModifier = max(min(volumeModifier, 1.0f), 0.0f);
					float vol = volumeModifier * m_volume;
					m_soundEffectInstance->SetVolume(vol);
				
				}
			}

			void SoundComponent::Play()
			{
				if (m_clip != nullptr)
				{
					m_soundEffectInstance->Play(m_looping);
					SetVolume(m_volume);
				}
			}

			void SoundComponent::PlayOneShot()
			{
				if (m_clip != nullptr)
				{
					if (m_is3D && AudioListener::GetInstance())
					{

						float range = 10.0f;
						float volumeModifier = range/Vector3::Distance(m_emitter->Position, AudioListener::GetInstance()->GetListner().Position);
						volumeModifier = max(min(volumeModifier, 1.0f), 0.0f);
						float vol = volumeModifier * m_volume;
						Sound::Play(m_clip->GetName(), vol);
					}else

					{
						Sound::Play(m_clip->GetName(), m_volume);
					}
					
				}
			}

			void SoundComponent::Stop()
			{
				if (m_clip != nullptr)
				{
					m_soundEffectInstance->Stop();
				}
			}

			void SoundComponent::Pause()
			{
				if (m_clip != nullptr)
				{
					m_soundEffectInstance->Pause();
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
				if (m_clip)
				{
					if (m_soundEffectInstance)
					{
						
						m_soundEffectInstance->Stop();
						m_soundEffectInstance.release();
					}
					
					m_soundEffectInstance = m_clip->CreateSoundEffectInstance();
				}
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
