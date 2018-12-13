#include "SoundManager.hpp"

// Thomas
#include "Common.h"
#include "ThomasCore.h"
#include "object/GameObject.h"
#include "object/component/AudioListener.h"

namespace thomas
{
	SoundManager::SoundManager()
		: m_lock()
	{
	}
	void SoundManager::Init()
	{
		m_system = nullptr;

		ErrorCheck(FMOD::System_Create(&m_system));
		ErrorCheck(m_system->init(36, FMOD_INIT_3D_RIGHTHANDED, NULL));
	}

	void SoundManager::Update()
	{
		auto listener = object::component::AudioListener::GetInstance();

		// No velocity set on the listener
		if (listener != nullptr)
		{
			m_system->set3DListenerAttributes(0, &Vector3ToFmod(listener->m_gameObject->GetTransform()->GetPosition()), NULL,
				&Vector3ToFmod(listener->m_gameObject->GetTransform()->Forward()), &Vector3ToFmod(listener->m_gameObject->GetTransform()->Up()));
		}
		
		ErrorCheck(m_system->update());
	}

	void SoundManager::Destroy()
	{
		// Release sounds
		for (const auto& sound : m_sounds)
		{
			sound.second->release();
		}
	}

	void SoundManager::LoadSound(const std::string& id, const std::string& file, bool looping, bool stream)
	{
		// Load sound and set it as 2D by default
		FMOD_MODE eMode = FMOD_DEFAULT;

		eMode |= FMOD_2D;
		eMode |= looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		eMode |= stream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

		// Add the sound to the container
		FMOD::Sound* sound = nullptr;

		ErrorCheck(m_system->createSound(file.c_str(), eMode, nullptr, &sound));
		m_lock.lock();
		auto inserted = m_sounds.insert(std::make_pair(id, std::move(sound)));
		m_lock.unlock();

//#ifdef _DEBUG
//		assert(inserted.second);
//#endif
	}

	FMOD_VECTOR SoundManager::Vector3ToFmod(const Vector3& v)
	{
		return FMOD_VECTOR{ v.x, v.y, v.z };
	}

	FMOD::System* SoundManager::GetSystem() const
	{
		return m_system;
	}

	SoundManager* SoundManager::GetInstance()
	{
		static SoundManager instance;

		return &instance;
	}

	FMOD::Sound* SoundManager::GetSound(const std::string& name) const
	{
		m_lock.lock();
		auto found = m_sounds.find(name);
//#ifdef _DEBUG
//		assert(found != m_sounds.end());
//#endif
		m_lock.unlock();


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