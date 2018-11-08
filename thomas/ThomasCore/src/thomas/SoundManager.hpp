#pragma once

// C++
#include <string>
#include <map>

// Thomas
#include "utils/Math.h"

// Fmod
#include <fmod/fmod_studio.hpp>
#include <fmod/fmod.hpp>

namespace thomas
{
	using namespace math;

	class SoundManager
	{
	public:
		using Channels = std::map<int, FMOD::Channel*>;

	private:
		SoundManager();
		~SoundManager() = default;
		SoundManager(const SoundManager&) {}
		SoundManager& operator = (const SoundManager&) {}
		SoundManager(const SoundManager&&) {}
		SoundManager& operator = (const SoundManager&&) {}

	public:
		void Update();
		void Destroy();
		void Play(const std::string& id, float volumedB); // Vector3
		void LoadSound(const std::string& id, const std::string& file, bool looping = false, bool stream = false);
		void SetChannelVolume(int channelId, float volumedB);
		float dbToVolume(float dB);
		float VolumeTodB(float volume);
		FMOD_VECTOR Vector3ToFmod(const Vector3& v);
		static SoundManager* GetInstance();

	private:
		bool ErrorCheck(FMOD_RESULT result);

		FMOD::Studio::System* m_studioSystem;
		FMOD::System* m_system;
		int m_nextChannelId;
		std::map<std::string, FMOD::Sound*> m_sounds;
		Channels m_channels;
	};
}