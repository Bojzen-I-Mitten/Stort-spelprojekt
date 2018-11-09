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
		struct Info
		{
			FMOD::Sound* sound = nullptr;
			FMOD::Channel* channel = nullptr;
			bool looping = false;
			float volume = 1.f;
		};

	private:
		SoundManager() = default;
		~SoundManager() = default;
		SoundManager(const SoundManager&) {}
		SoundManager& operator = (const SoundManager&) {}
		SoundManager(const SoundManager&&) {}
		SoundManager& operator = (const SoundManager&&) {}

	public:
		void Init();
		void Update();
		void Destroy();
		void Play(const std::string& id);
		void LoadSound(const std::string& id, const std::string& file, bool looping = false, bool stream = false);
		FMOD_VECTOR Vector3ToFmod(const Vector3& v);
		Info& GetSoundInfo(const std::string& name);
		static SoundManager* GetInstance();

	private:
		bool ErrorCheck(FMOD_RESULT result);

		FMOD::Studio::System* m_studioSystem;
		FMOD::System* m_system;
		std::map<std::string, Info> m_sounds;
	};
}