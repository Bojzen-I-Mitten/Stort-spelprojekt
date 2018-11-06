#include "SoundComponent.h"

#include "../../Sound.h"
#include "../../resource/AudioClip.h"
#include "../../ThomasTime.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			SoundComponent::SoundComponent() :
			m_looping(true),
			m_clip(nullptr),
			m_volume(0.5f)
			{
			}

			void SoundComponent::OnDisable()
			{
				Stop(); // Would be better to just use the sound engine to stop when not in play mode and then resume...
			}

			void SoundComponent::Apply3D(const Vector3& listenerPos, const Vector3& sourcePos)
			{
				EDITOR_LOCK();
				// Apply 3D-effect with attenuation formula based on Inverse Square Law
				if (m_clip != nullptr)
				{
					float attenuation = Sound::VolumeTodB(m_volume) - Sound::VolumeTodB((sourcePos - listenerPos).Length());
					m_clip->GetSoundEffectInstance()->SetVolume(Sound::dbToVolume(attenuation));
				}
			}

			void SoundComponent::Play()
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Play(m_looping);
				}
			}

			void SoundComponent::PlayOneShot()
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					Sound::Play(m_clip->GetName(), m_volume);
				}
			}

			void SoundComponent::Stop()
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Stop();
				}
			}

			void SoundComponent::Pause()
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Pause();
				}
			}

			void SoundComponent::Resume()
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					m_clip->GetSoundEffectInstance()->Resume();
				}
			}

			bool SoundComponent::IsPlaying() const
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					return Sound::IsPlaying(m_clip->GetName());	
				}

				return false;
			}

			bool SoundComponent::IsPaused() const
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					return Sound::IsPaused(m_clip->GetName());
				}

				return false;
			}

			bool SoundComponent::HasStopped() const
			{
				EDITOR_LOCK();
				if (m_clip != nullptr)
				{
					return Sound::HasStopped(m_clip->GetName());
				}

				return false;
			}

			void SoundComponent::SetClip(resource::AudioClip* clip)
			{
				EDITOR_LOCK();
				m_clip = clip;
			}

			void SoundComponent::SetVolume(float volume)
			{
				EDITOR_LOCK();
				if (volume <= 5.f && volume >= 0.f)
				{
					m_volume = volume;

					if (m_clip != nullptr)
					{
						m_clip->GetSoundEffectInstance()->SetVolume(Sound::dbToVolume(m_volume));
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
