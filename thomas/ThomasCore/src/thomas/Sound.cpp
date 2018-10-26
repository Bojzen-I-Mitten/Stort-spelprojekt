#include "Sound.h"
#include <AtlBase.h>
#include <atlconv.h>

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

			for (auto& sound : s_waves)
			{
				sound.second.soundEffectInstance.reset();
				sound.second.soundEffect.reset();
			}
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

	void Sound::Suspend()
	{
		// Should be used if pausing a game -> resume
		s_audioEngine->Suspend();
	}

	bool Sound::IsPlaying(const std::string & name)
	{
		if (!s_waves.empty())
		{
			if (GetSoundInfo(name).soundEffectInstance->GetState() == SoundState::PLAYING)
			{
				return true;
			}
		}

		return false;
	}

	bool Sound::IsPaused(const std::string& name)
	{
		if (!s_waves.empty())
		{
			if (GetSoundInfo(name).soundEffectInstance->GetState() == SoundState::PAUSED)
			{
				return true;
			}
		}

		return false;
	}

	bool Sound::HasStopped(const std::string& name)
	{
		if (!s_waves.empty())
		{
			if (GetSoundInfo(name).soundEffectInstance->GetState() == SoundState::STOPPED)
			{
				return true;
			}
		}

		return false;
	}

	void Sound::LoadSound(const std::string& name, const std::string& file)
	{
		SoundInfo info;
		info.soundEffect = std::make_unique<SoundEffect>(s_audioEngine.get(), CA2W(file.c_str()));
		info.soundEffectInstance = info.soundEffect->CreateInstance();

		s_waves.insert(std::make_pair(name, std::move(info)));
	}

	void Sound::Play(const std::string& name, float volume)
	{
		// Play a oneshot
		if (!s_waves.empty())
		{
			GetSoundInfo(name).soundEffect->Play(volume, 0.f, 0.f);
		}
	}

	float Sound::dbToVolume(float dB)
	{
		return powf(10.f, 0.05f * dB);
	}

	float Sound::VolumeTodB(float volume)
	{
		return 20.f * log10f(volume);
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

	Sound::SoundInfo& Sound::GetSoundInfo(const std::string& name)
	{
		auto found = s_waves.find(name);

#ifdef _DEBUG
		assert(found != s_waves.end());
#endif

		// TODO: Better error checking, but clips should never be removed during runtime
		return found->second;
	}
}