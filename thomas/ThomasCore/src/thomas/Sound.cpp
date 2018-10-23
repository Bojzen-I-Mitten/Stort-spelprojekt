#include "Sound.h"
#include <AtlBase.h>
#include <atlconv.h>

// Note: could have a separate thread for audio updates?
namespace thomas
{
	std::unique_ptr<AudioEngine> Sound::s_audioEngine;
	std::map<std::string, std::unique_ptr<SoundEffect>> Sound::s_waves;
	float Sound::s_masterVolume;
	float Sound::s_fxVolume;
	float Sound::s_musicVolume;

	void Sound::Init()
	{
		CoInitialize(NULL);
		AUDIO_ENGINE_FLAGS aEFlags = AudioEngine_Default;

#ifdef _DEBUG
		aEFlags = aEFlags | AudioEngine_Debug;
#endif

		// Init audio engine
		s_audioEngine = std::make_unique<AudioEngine>(aEFlags);

		// TODO: check if there is a audio device available?

		// Init volumes to default values
		s_masterVolume = 0.5f;
		s_fxVolume = 0.5f;
		s_musicVolume = 0.5f;
	}

	void Sound::Destroy()
	{
		if (s_audioEngine)
		{
			s_audioEngine->Suspend();
		}
	}

	void Sound::Update()
	{
		if (!s_audioEngine->Update())
		{
			// Failed to update audio engine
		}
	}

	void Sound::Resume()
	{
		s_audioEngine->Resume();
	}

	void Sound::LoadSound(const std::string& name, const std::string& file)
	{
		s_waves.insert(std::make_pair(name, std::make_unique<SoundEffect>(s_audioEngine.get(), CA2W(file.c_str()))));
	}

	void Sound::Play(const std::string& name, float volume)
	{
		// Play a oneshot
		if (!s_waves.empty())
		{
			/*if (s_audioEngine->Reset())
			{

			}*/

			GetSoundInfo(name)->Play(s_masterVolume * s_fxVolume * volume, 0.f, 0.f);
		}
	}

	std::unique_ptr<SoundEffectInstance> Sound::CreateInstance(const std::string& name, SOUND_EFFECT_INSTANCE_FLAGS flags)
	{
		auto found = s_waves.find(name);

#ifdef _DEBUG
		assert(found != s_waves.end());
#endif

		// TODO: Better error checking 
		return found->second->CreateInstance(flags);
	}

	SoundEffect* Sound::GetSound(const std::string& name)
	{
		return GetSoundInfo(name);
	}

	void Sound::SetMasterVolume(float volume)
	{
		s_masterVolume = volume;
	}

	void Sound::SetFxVolume(float volume)
	{
		s_fxVolume = volume;
	}

	void Sound::SetMusicVolume(float volume)
	{
		s_musicVolume = volume;
	}

	float Sound::GetMasterVolume()
	{
		return s_masterVolume;
	}

	float Sound::GetMusicVolume()
	{
		return s_musicVolume;
	}

	float Sound::GetFxVolume()
	{
		return s_fxVolume;
	}

	SoundEffect* Sound::GetSoundInfo(const std::string& name)
	{
		auto found = s_waves.find(name);

#ifdef _DEBUG
		assert(found != s_waves.end());
#endif

		// TODO: Better error checking, but clips should never be removed during runtime
		return found->second.get();
	}
}