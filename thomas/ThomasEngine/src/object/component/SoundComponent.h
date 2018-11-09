#pragma once

#include "../Component.h"

namespace thomas { namespace object { namespace component { class SoundComponent; } } }

namespace ThomasEngine
{
	ref class AudioClip;
	public ref class SoundComponent : public Component
	{
	private:
		AudioClip^ m_clip;
		property thomas::object::component::SoundComponent* sound { thomas::object::component::SoundComponent* get(); }

	public:
		SoundComponent();

		void Play();
		void Stop();

		property AudioClip^ clip
		{
			AudioClip^ get();
			void set(AudioClip^ value);
		}

		property float Volume
		{
			float get();
			void set(float value);
		}

		property bool Looping
		{
			bool get();
			void set(bool value);
		}
	};
}