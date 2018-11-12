#pragma once

// Thomas
#include "Component.h"
//#include "../../utils/Math.h"

namespace thomas
{
	//using namespace math;

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
				// The sound component should have a channel instead? This should solve the multiple position issue...
				resource::AudioClip* m_clip;

				// These variables are a bit unconvenient but if we don't have them
				// the user won't be able to set "properties of a clip" in the inspector before the clip is set
				bool m_is3D;
				bool m_looping;
				bool m_paused;
				float m_volume;
			};
		}
	}
}