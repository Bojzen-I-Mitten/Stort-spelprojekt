#include "SoundManager.hpp"

// Thomas
#include "Common.h"
#include "ThomasCore.h"
#include "object/GameObject.h"
#include "object/component/AudioListener.h"

namespace thomas
{
	void SoundManager::Init()
	{
		m_system = nullptr;
		m_studioSystem = nullptr;

		ErrorCheck(FMOD::Studio::System::create(&m_studioSystem));
		ErrorCheck(m_studioSystem->initialize(32, NULL, NULL, NULL));
		ErrorCheck(m_studioSystem->getLowLevelSystem(&m_system));
	}

	void SoundManager::Update()
	{
		auto listener = object::component::AudioListener::GetInstance();

		// No velocity set on the listener
		if (listener != nullptr)
		{
			m_system->set3DListenerAttributes(0, &Vector3ToFmod(listener->m_gameObject->m_transform->GetPosition()), NULL,
				&Vector3ToFmod(listener->m_gameObject->m_transform->Forward()), &Vector3ToFmod(listener->m_gameObject->m_transform->Up()));
		}
		
		ErrorCheck(m_system->update());
		ErrorCheck(m_studioSystem->update());
	}

	void SoundManager::Destroy()
	{
		// Release sounds
		for (const auto& sound : m_sounds)
		{
			sound.second.sound->release();
		}

		ErrorCheck(m_studioSystem->unloadAll());
		ErrorCheck(m_studioSystem->release());
	}

	void SoundManager::Play(const std::string& id)
	{
		auto& found = GetSoundInfo(id);

		// Set looping options
		// Note: if you put a sound on looping during runtime and which is playing
		// it will play separately from the same sound instance which is not set to looping
		// this behavior can then be repeated
		if (found.looping)
		{
			found.sound->setMode(FMOD_LOOP_NORMAL);
			found.sound->setLoopCount(-1); // Loop repeatedly
		}
		else
		{
			found.sound->setMode(FMOD_LOOP_OFF);
		}

		ErrorCheck(m_system->playSound(found.sound, nullptr, found.paused, &found.channel));

		if (found.channel != nullptr)
		{
			// Set channel properties
			ErrorCheck(found.channel->setVolume(found.volume));
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
			LOG("FMOD ERROR " << result);
			return false;
		}

		return true;
	}
}