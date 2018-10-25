#pragma once

#include "Component.h"
#include "../../utils/Math.h"

#include <memory>

namespace thomas
{
	using namespace math;

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
				void Apply3D(const Vector3& listenerPos, const Vector3& sourcePos);
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