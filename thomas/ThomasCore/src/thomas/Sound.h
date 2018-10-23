#pragma once

#include <include/DirectXTK/SoundCommon.h>
#include <map>

using namespace DirectX;

namespace thomas
{
	class Sound
	{
	public:
		static void Init();
		static void Destroy();
		static void Update();
		static void Resume();

		static void Play(const std::string& name, float volume);
		static void LoadSound(const std::string& name, const std::string& file);
		static std::unique_ptr<SoundEffectInstance> CreateInstance(const std::string& name, 
													SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);

	public:
		static void SetMasterVolume(float volume);
		static void SetFxVolume(float volume);
		static void SetMusicVolume(float volume);

	public:
		static SoundEffect* GetSound(const std::string& name);
		static float GetMasterVolume();
		static float GetMusicVolume();
		static float GetFxVolume();

	private:
		static SoundEffect* GetSoundInfo(const std::string& name);

	private:
		static std::unique_ptr<AudioEngine> s_audioEngine;
		static std::map<std::string, std::unique_ptr<SoundEffect>> s_waves;
		static float s_masterVolume;
		static float s_fxVolume;
		static float s_musicVolume;
	};
}