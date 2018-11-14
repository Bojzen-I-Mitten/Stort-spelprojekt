#pragma once

#include "Component.h"
#include "../../utils/Math.h"
#include "Component.h"
#include <memory>

namespace DirectX { class AudioEmitter; class SoundEffectInstance; }
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
				~SoundComponent();

				virtual void OnDisable() override;
				void Update();
				void Play();
				void PlayOneShot();
				void Stop();
				void Pause();
				void Resume();

				bool IsPlaying() const;
				bool IsPaused() const;
				bool HasStopped() const;
			private:
				void Apply3D(const Vector3& listenerPos, const Vector3& sourcePos);

			public:
				void SetClip(resource::AudioClip* clip);		
				void SetVolume(float volume);
				void SetVolumeFactor(float volumeFactor);
				void SetLooping(bool looping);
				void Set3D(bool value);
			public:
				resource::AudioClip* GetClip() const;
				float GetVolume() const;
				float GetVolumeFactor() const;
				bool IsLooping() const;
				bool is3D() const;
			private:
				resource::AudioClip* m_clip;
				float m_volume;
				float m_volumeFactor;
				bool m_looping;
				bool m_is3D;
				DirectX::AudioEmitter* m_emitter;
				std::unique_ptr<DirectX::SoundEffectInstance> m_soundEffectInstance;
			};
		}
	}
}