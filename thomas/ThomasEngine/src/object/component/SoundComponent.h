#pragma once

#include "../Component.h"

namespace thomas { namespace object { namespace component { class SoundComponent; } } }

namespace ThomasEngine
{
	ref class AudioClip;
	public ref class SoundComponent : public Component
	{
	public:
		enum class SoundType
		{
			Music,
			Effect,
			Voice
		};

	private:
		AudioClip^ m_clip;
		property thomas::object::component::SoundComponent* sound { thomas::object::component::SoundComponent* get(); }

	public:
		SoundComponent();

		void Play();
		void Pause(bool paused);
		void Mute(bool mute);
		void Stop();
		bool IsPlaying();
		bool IsPaused();
		bool IsMuted();

		property SoundType Type
		{
			SoundType get();
			void set(SoundType value);
		}

		property AudioClip^ Clip
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

		property bool Is3D
		{
			bool get();
			void set(bool value);
		}

		[DisplayNameAttribute("Min Distance")]
		[CategoryAttribute("3D")]
		property float MinDistance
		{
			float get();
			void set(float value);
		}

		[DisplayNameAttribute("Max Distance")]
		[CategoryAttribute("3D")]
		property float MaxDistance
		{
			float get();
			void set(float value);
		}

		[DisplayNameAttribute("Spread Angle")]
		[CategoryAttribute("3D")]
		property float SpreadAngle
		{
			float get();
			void set(float value);
		}
	};
}