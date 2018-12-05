#pragma once

// C++
#include <string>
#include <map>

// Thomas
#include "utils/Math.h"
#include "utils/atomic/Synchronization.h"

// Fmod
#include <fmod/fmod_studio.hpp>
#include <fmod/fmod.hpp>

namespace thomas
{
	using namespace math;

	class SoundManager
	{
	private:
		SoundManager();
		~SoundManager() = default;
		SoundManager(const SoundManager&) {}
		SoundManager& operator = (const SoundManager&) {}
		SoundManager(const SoundManager&&) {}
		SoundManager& operator = (const SoundManager&&) {}

	public:
		void Init();
		void Update();
		void Destroy();
		void LoadSound(const std::string& id, const std::string& file, bool looping = false, bool stream = false);
		FMOD_VECTOR Vector3ToFmod(const Vector3& v);
		FMOD::System* GetSystem() const;
		FMOD::Sound* GetSound(const std::string& name) const;
		static SoundManager* GetInstance();

	private:
		bool ErrorCheck(FMOD_RESULT result);

		FMOD::Studio::System* m_studioSystem;
		FMOD::System* m_system;
		std::map<std::string, FMOD::Sound*> m_sounds;
		mutable utils::atomics::SpinLock m_lock;		// Simple resource lock for multi-threaded access
	};
}