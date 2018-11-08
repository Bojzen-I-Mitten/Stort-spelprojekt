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
		//Check if a channel has stopped playing and destroy it
		std::vector<Channels::const_iterator> stoppedChannels;

		for (auto it = m_channels.begin(), itEnd = m_channels.end(); it != itEnd; ++it)
		{
			bool playing = false;
			it->second->isPlaying(&playing);

			if (!playing)
			{
				stoppedChannels.push_back(it);
			}
		}

		for (const auto& it : stoppedChannels)
		{
			m_channels.erase(it);
		}

		ErrorCheck(m_system->update());
		ErrorCheck(m_studioSystem->update());
	}

	void SoundManager::Destroy()
	{
		// Release sounds
		for (const auto& sound : m_sounds)
			sound.second->release();

		ErrorCheck(m_studioSystem->unloadAll());
		ErrorCheck(m_studioSystem->release());
	}

	void SoundManager::Play(const std::string& id, float volumedB)
	{
		// Need to keep track of this better...
		int nChannelId = m_nextChannelId++;
		auto tFoundIt = m_sounds.find(id);

		FMOD::Channel* pChannel = nullptr;

		ErrorCheck(m_system->playSound(tFoundIt->second, nullptr, true, &pChannel));

		if (pChannel)
		{
			ErrorCheck(pChannel->setVolume(dbToVolume(volumedB)));
			ErrorCheck(pChannel->setPaused(false));

			auto inserted = m_channels.insert(std::make_pair(nChannelId, pChannel));

#ifdef _DEBUG
			assert(inserted.second);
#endif
		}

		//return nChannelId;
	}

	void SoundManager::LoadSound(const std::string& id, const std::string& file, bool looping, bool stream)
	{
		// Try loading 2D sounds for now
		FMOD_MODE eMode = FMOD_DEFAULT;

		eMode |= FMOD_2D;
		eMode |= looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		FMOD::Sound* pSound = nullptr;
		ErrorCheck(m_system->createSound(file.c_str(), eMode, nullptr, &pSound));
		auto inserted = m_sounds.insert(std::make_pair(id, std::move(pSound)));

#ifdef _DEBUG
		assert(inserted.second);
#endif
	}

	void SoundManager::SetChannelVolume(int channelId, float volumedB)
	{
		auto tFoundIt = m_channels.find(channelId);
		if (tFoundIt == m_channels.end())
			return;

		ErrorCheck(tFoundIt->second->setVolume(dbToVolume(volumedB)));
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