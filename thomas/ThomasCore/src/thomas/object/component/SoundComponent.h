#pragma once

// Fmod
#include <fmod/fmod.hpp>

// Thomas
#include "Component.h"

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
				void Stop();
				void Update();

			public:
				void SetClip(resource::AudioClip* clip);
				void SetVolume(float volume);
				void Set3D(bool is3D);
				void SetLooping(bool looping);
				void SetPaused(bool paused);

			public:
				resource::AudioClip* GetClip() const;
				float GetVolume() const;
				bool IsPlaying() const;
				bool IsLooping() const;
				bool IsPaused() const;
				bool Is3D() const;

			private:
				FMOD::Channel* m_channel;
				resource::AudioClip* m_clip;
				bool m_is3D;
				bool m_looping;
				bool m_paused;
				float m_volume;
			};
		}
	}
}