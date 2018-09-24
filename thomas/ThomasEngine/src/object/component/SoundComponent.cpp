#pragma unmanaged
#include <thomas\object\component\SoundComponent.h>
#pragma managed
#include "../../resource/AudioClip.h"
#include "SoundComponent.h"
namespace ThomasEngine
{
	SoundComponent::SoundComponent() : Component(new thomas::object::component::SoundComponent()) {}
	thomas::object::component::SoundComponent* SoundComponent::sound::get() { return (thomas::object::component::SoundComponent*)nativePtr; }

	AudioClip^ SoundComponent::clip::get() { return m_clip; }
	void SoundComponent::clip::set(AudioClip^ value)
	{
		m_clip = value;
		sound->SetClip((thomas::resource::AudioClip*)value->m_nativePtr);
	}

	float SoundComponent::Volume::get() { return sound->GetVolume(); }
	void SoundComponent::Volume::set(float value) { sound->SetVolume(value); }

	bool SoundComponent::Looping::get() { return sound->IsLooping(); }
	void SoundComponent::Looping::set(bool value) { sound->SetLooping(value); }

	bool SoundComponent::Play() { return sound->Play(); }
	bool SoundComponent::PlayOneShot(String^ name, float volume) { return sound->PlayOneShot(Utility::ConvertString(name), volume); }
	void SoundComponent::Pause() { return sound->Pause(); }
	void SoundComponent::Resume() { return sound->Resume(); }
}