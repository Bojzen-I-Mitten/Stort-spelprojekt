#pragma unmanaged
#include <thomas/object/component/SoundComponent.h>

#pragma managed
#include "../../resource/AudioClip.h"
#include "SoundComponent.h"

namespace ThomasEngine
{
	SoundComponent::SoundComponent() : Component(new thomas::object::component::SoundComponent()) {}
	thomas::object::component::SoundComponent* SoundComponent::sound::get() { return (thomas::object::component::SoundComponent*)nativePtr; }


	void SoundComponent::Play()
	{
		sound->Play();
	}

	void SoundComponent::PlayOneShot()
	{
		sound->PlayOneShot();
	}

	void SoundComponent::Stop()
	{
		sound->Stop();
	}

	void SoundComponent::Pause()
	{
		sound->Pause();
	}

	void SoundComponent::Resume()
	{
		sound->Resume();
	}

	bool SoundComponent::IsPlaying()
	{
		return sound->IsPlaying();
	}

	bool SoundComponent::IsPaused()
	{
		return sound->IsPaused();
	}

	bool SoundComponent::HasStopped()
	{
		return sound->HasStopped();
	}

	bool SoundComponent::Is3D::get()
	{
		return sound->is3D();
	}
	void SoundComponent::Is3D::set(bool value)
	{
		sound->Set3D(value);
	}

	void SoundComponent::clip::set(AudioClip^ value)
	{
		m_clip = value;
		sound->SetClip((thomas::resource::AudioClip*)value->m_nativePtr);
	}

	AudioClip^ SoundComponent::clip::get() 
	{ 
		return m_clip; 
	}

	void SoundComponent::Volume::set(float value)
	{
		sound->SetVolume(value);
	}

	float SoundComponent::Volume::get() 
	{ 
		return sound->GetVolume(); 
	}

	void SoundComponent::VolumeFactor::set(float value)
	{
		sound->SetVolumeFactor(value);
	}

	float SoundComponent::VolumeFactor::get()
	{
		return sound->GetVolumeFactor();
	}

	void SoundComponent::Looping::set(bool value) 
	{ 
		sound->SetLooping(value); 
	}

	bool SoundComponent::Looping::get() 
	{ 
		return sound->IsLooping(); 
	}
}