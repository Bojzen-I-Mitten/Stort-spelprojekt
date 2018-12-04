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

	enum class SoundType
	{
		Music,
		Effect,
		Voice
	};

	namespace object
	{
		namespace component
		{
			class SoundComponent : public Component
			{
			public:
				SoundComponent();
				virtual ~SoundComponent() = default;

				virtual void OnDisable() override;
				virtual void OnDestroy() override;
				void Play();
				void Play(resource::AudioClip* clip, float volume, bool looping, bool is3D);
				void Stop();
				virtual void Update() override;

			public:
				void SetType(SoundType type);
				void SetClip(resource::AudioClip* clip);
				void SetVolume(float volume);
				void Set3D(bool is3D);
				void SetLooping(bool looping);
				void SetPaused(bool paused);
				void SetMute(bool mute);
				void Set3DMinDistance(float min);
				void Set3DMaxDistance(float max);
				void Set3DSpreadAngle(float angle);

			public:
				SoundType GetType() const;
				resource::AudioClip* GetClip() const;
				float GetVolume() const;
				float Get3DMinDistance() const;
				float Get3DMaxDistance() const;
				float Get3DSpreadAngle() const;
				bool IsPlaying() const;
				bool IsLooping() const;
				bool IsPaused() const;
				bool Is3D() const;
				bool IsMute() const;

			private:
				void AdjustVolumeType(float volume);

			private:
				SoundType m_type;
				FMOD::Channel* m_channel;
				resource::AudioClip* m_clip;
				bool m_is3D;
				bool m_looping;
				bool m_paused;
				bool m_muted;
				float m_volume;
				float m_minDistance;
				float m_maxDistance;
				float m_spreadAngle;
			};
		}
	}
}