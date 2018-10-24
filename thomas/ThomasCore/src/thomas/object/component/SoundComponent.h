#pragma once

#include "Component.h"
#include "../../utils/Math.h"

#include <memory>
#include <include/DirectXTK/SoundCommon.h>

using namespace DirectX;

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
				void Play3D(const Vector3& listener, const Vector3& emitter);
				void Play2D();
				void PlayOneShot2D();
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
				AudioEmitter m_audioEmitter;
				AudioListener m_listener;
				float m_volume;
				bool m_looping;
			};
		}
	}
}