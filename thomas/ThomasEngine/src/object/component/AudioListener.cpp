#pragma unmanaged
#include <thomas/object/component/AudioListener.h>
#pragma managed
#include "AudioListener.h"

namespace ThomasEngine
{
	AudioListener::AudioListener() : Component(new thomas::object::component::AudioListener()) {}
	thomas::object::component::AudioListener* AudioListener::listener::get() { return (thomas::object::component::AudioListener*)nativePtr; }

	void AudioListener::SetMasterVolume(float volume)
	{
		listener->SetMasterVolume(volume);
	}

	void AudioListener::SetMusicVolume(float volume)
	{
		listener->SetMusicVolume(volume);
	}

	void AudioListener::SetFXVolume(float volume)
	{
		listener->SetFXVolume(volume);
	}

	void AudioListener::SetVoiceVolume(float volume)
	{
		listener->SetVoiceVolume(volume);
	}

	float AudioListener::GetMasterVolume()
	{
		return listener->GetMasterVolume();
	}

	float AudioListener::GetMusicVolume()
	{
		return listener->GetMusicVolume();
	}

	float AudioListener::GetFXVolume()
	{
		return listener->GetFXVolume();
	}

	float AudioListener::GetVoiceVolume()
	{
		return listener->GetVoiceVolume();
	}
}