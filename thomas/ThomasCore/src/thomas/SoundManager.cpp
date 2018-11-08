#include "SoundManager.hpp"

// Thomas
#include "Common.h"
#include "ThomasCore.h"

namespace thomas
{
	SoundManager::SoundManager() :
	m_system(nullptr),
	m_studioSystem(nullptr)
	{
		ErrorCheck(FMOD::Studio::System::create(&m_studioSystem));
		ErrorCheck(m_studioSystem->initialize(32, NULL, NULL, NULL));
		ErrorCheck(m_studioSystem->getLowLevelSystem(&m_system));
	}

	void SoundManager::Update()
	{
		// Check if a channel has stopped playing and invalidate it
		for (auto& sound : m_sounds)
		{
			bool playing = false;
			sound.second.channel->isPlaying(&playing);

			if (!playing)
			{
				sound.second.channel = nullptr;
			}
		}

		ErrorCheck(m_system->update());
		ErrorCheck(m_studioSystem->update());
	}

	void SoundManager::Destroy()
	{
		// Release sounds
		for (const auto& sound : m_sounds)
			sound.second.sound->release();

		ErrorCheck(m_studioSystem->unloadAll());
		ErrorCheck(m_studioSystem->release());
	}

	void SoundManager::Play(const std::string& id)
	{
		auto found = m_sounds.find(id);

		ErrorCheck(m_system->playSound(found->second.sound, nullptr, true, &found->second.channel));

		if (found->second.channel)
		{
			//ErrorCheck(found->second.channel->setVolume(dbToVolume(volumedB)));
			ErrorCheck(found->second.channel->setPaused(false));
		}
	}

	void SoundManager::LoadSound(const std::string& id, const std::string& file, bool looping, bool stream)
	{
		// Try loading 2D sounds for now
		FMOD_MODE eMode = FMOD_DEFAULT;

		eMode |= FMOD_2D;
		eMode |= looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		// Add the sound to the container
		Info soundInfo;
		ErrorCheck(m_system->createSound(file.c_str(), eMode, nullptr, &soundInfo.sound));
		auto inserted = m_sounds.insert(std::make_pair(id, std::move(soundInfo)));

#ifdef _DEBUG
		assert(inserted.second);
#endif
	}

	float SoundManager::dbToVolume(float dB)
	{
		return powf(10.f, 0.05f * dB);
	}

	float SoundManager::VolumeTodB(float volume)
	{
		return 20.f * log10f(volume);
	}

	FMOD_VECTOR SoundManager::Vector3ToFmod(const Vector3& v)
	{
		return FMOD_VECTOR{ v.x, v.y, v.z };
	}

	SoundManager* SoundManager::GetInstance()
	{
		static SoundManager instance;

		return &instance;
	}

	SoundManager::Info& SoundManager::GetSoundInfo(const std::string& name)
	{
		auto found = m_sounds.find(name);

#ifdef _DEBUG
		assert(found != m_sounds.end());
#endif

		return found->second;
	}

	bool SoundManager::ErrorCheck(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			LOG("FMOD ERROR ", result);
			return false;
		}

		return true;
	}
}