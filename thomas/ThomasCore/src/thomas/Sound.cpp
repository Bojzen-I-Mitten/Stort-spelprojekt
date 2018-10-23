#include "Sound.h"

// Note: could have a separate thread for audio updates?
namespace thomas
{
	std::unique_ptr<AudioEngine> Sound::s_audioEngine;
	std::map<std::string, Sound::SoundInfo> Sound::s_waves;
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

	void Sound::Play(const std::string& file)
	{
		// Play a oneshot
		if (s_waves.empty())
		{
			/*if (s_audioEngine->Reset())
			{

			}*/

			auto& sound = GetSoundInfo(file);
			sound.soundEffect->Play(s_masterVolume * s_fxVolume * sound.volume, sound.pitch, sound.pan);
		}
	}

	void Sound::LoadSound(const std::string& file)
	{
		SoundInfo info = { std::make_unique<SoundEffect>(s_audioEngine.get(), file.c_str()), 1.f, 0.f, 0.f };
		s_waves.insert(std::make_pair(file, info));
	}

	void Sound::CreateInstance(const std::string& clipName, std::unique_ptr<SoundEffectInstance> instance, 
							   SOUND_EFFECT_INSTANCE_FLAGS flags)
	{
		auto found = s_waves.find(clipName);

#ifdef _DEBUG
		assert(found != s_waves.end());
#endif

		// TODO: better error checking 
		instance = s_waves.find(clipName)->second.soundEffect->CreateInstance(flags);
	}

	SoundEffect* Sound::GetSound(const std::string& file)
	{
		auto& sound = GetSoundInfo(file);
		return sound.soundEffect.get();
	}

	float Sound::GetClipVolume(const std::string& file)
	{
		auto& sound = GetSoundInfo(file);
		return sound.volume;
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

	void Sound::SetClipVolume(const std::string& file, float volume)
	{
		auto& sound = GetSoundInfo(file);
		sound.volume = volume;
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

	Sound::SoundInfo& Sound::GetSoundInfo(const std::string& file)
	{
		auto found = s_waves.find(file);

#ifdef _DEBUG
		assert(found != s_waves.end());
#endif

		// TODO: better error checking, but clips should never be removed during runtime
		return found->second;
	}
}