#pragma once

#include "../Component.h"

namespace thomas { namespace object { namespace component { class AudioListener; } } }

namespace ThomasEngine
{
	public ref class AudioListener : public Component
	{
	private:
		property thomas::object::component::AudioListener* listener { thomas::object::component::AudioListener* get(); }

	public:
		AudioListener();

	public:
		void SetMasterVolume(float volume);
		void SetMusicVolume(float volume);
		void SetFXVolume(float volume);
		void SetVoiceVolume(float volume);

	public:
		float GetMasterVolume();
		float GetMusicVolume();
		float GetFXVolume();
		float GetVoiceVolume();
	};
}