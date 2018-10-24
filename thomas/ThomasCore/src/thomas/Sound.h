#pragma once

#include <include/DirectXTK/SoundCommon.h>
#include <map>

using namespace DirectX;

namespace thomas
{
	class Sound
	{
	private:
		struct SoundInfo
		{
			std::unique_ptr<SoundEffect> soundEffect;
			std::unique_ptr<SoundEffectInstance> soundEffectInstance;
		};

	public:
		static void Init();
		static void Destroy();
		static void Update();
		static void Resume();
		static void Suspend();
		static bool IsPlaying(const std::string& name);
		static bool IsPaused(const std::string& name);
		static bool HasStopped(const std::string& name);

		static void Play(const std::string& name, float volume);
		static void LoadSound(const std::string& name, const std::string& file);

	public:
		static void SetMasterVolume(float volume);
		static void SetFxVolume(float volume);
		static void SetMusicVolume(float volume);

	public:
		static float GetMasterVolume();
		static float GetMusicVolume();
		static float GetFxVolume();
		static SoundInfo& GetSoundInfo(const std::string& name);

	private:
		static std::unique_ptr<AudioEngine> s_audioEngine;
		static std::map<std::string, SoundInfo> s_waves;
		static float s_masterVolume;
		static float s_fxVolume;
		static float s_musicVolume;
	};
}