#pragma once
#include "DirectXTK\SoundCommon.h"
#include <map>

using namespace DirectX;

namespace thomas
{
	class Sound
	{
	public:
		bool Init();
		bool Play(const std::string & name, const float & volume);
		std::unique_ptr<SoundEffectInstance> CreateInstance(const std::string & clipName, SOUND_EFFECT_INSTANCE_FLAGS flags);
		bool LoadWaveBank(const std::string & name);
		std::unique_ptr<SoundEffect> LoadWave(const std::string & path);
		void Destroy();
		void Update();

	public:
		void SetMasterVolume(const float & volume);
		void SetFxVolume(const float & volume);
		void SetMusicVolume(const float & volume);

	public:
		static Sound* Instance();
		float GetMusicVolume();

	private:
		float s_masterVolume;
		float s_fxVolume;
		float s_musicVolume;

	private:
		std::unique_ptr<WaveBank> s_bank;
		std::unique_ptr<AudioEngine> s_audioEngine;
		std::map<std::string, std::unique_ptr<SoundEffect>> s_waves;

		static Sound s_sound;
	};
}