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

				// These variables are a bit unconvenient but if we don't have them
				// the user won't be able to set "properties of a clip" in the inspector before the clip is set
				bool m_looping;
				float m_volume;
			};
		}
	}
}