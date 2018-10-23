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
			float volume;
			float pitch;
			float pan;
		};

	public:
		static void Init();
		static void Destroy();
		static void Update();
		static void Resume();

		void Play(const std::string& file);
		void LoadSound(const std::string& file);
		void CreateInstance(const std::string& file, std::unique_ptr<SoundEffectInstance> instance,
							SOUND_EFFECT_INSTANCE_FLAGS flags = SoundEffectInstance_Default);

	public:
		static void SetMasterVolume(float volume);
		static void SetFxVolume(float volume);
		static void SetMusicVolume(float volume);
		static void SetClipVolume(const std::string& file, float volume);

	public:
		static SoundEffect* GetSound(const std::string& file);
		static float GetClipVolume(const std::string& file);
		static float GetMasterVolume();
		static float GetMusicVolume();
		static float GetFxVolume();

	private:
		static SoundInfo& GetSoundInfo(const std::string& file);

	private:
		static std::unique_ptr<AudioEngine> s_audioEngine;
		static std::map<std::string, SoundInfo> s_waves;
		static float s_masterVolume;
		static float s_fxVolume;
		static float s_musicVolume;
	};
}