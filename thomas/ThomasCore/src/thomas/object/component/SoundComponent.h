#pragma once

#include "Component.h"
#include <memory>

namespace thomas
{
	namespace resource
	{
		class AudioClip;
	}

	namespace object
	{
		namespace component
		{
			class SoundComponent : public Component
			{
			public:
				SoundComponent();
				~SoundComponent() = default;

				virtual void OnDisable() override;
				void Play();
				void PlayOneShot();
				void Stop();
				void Pause();
				void Resume();
				bool IsPlaying() const;
				bool IsPaused() const;
				bool HasStopped() const;

			public:
				void SetClip(resource::AudioClip* clip);		
				void SetVolume(float volume);
				void SetLooping(bool looping);

			public:
				resource::AudioClip* GetClip() const;
				float GetVolume() const;
				bool IsLooping() const;

			private:
				resource::AudioClip* m_clip;
				float m_volume;
				bool m_looping;
			};
		}
	}
}