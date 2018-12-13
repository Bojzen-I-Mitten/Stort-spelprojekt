#include "../../../../ThomasCore/src/thomas/object/component/SoundComponent.h"
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

	void SoundComponent::Stop()
	{
		sound->Stop();
	}

	bool SoundComponent::IsPlaying()
	{
		return sound->IsPlaying();
	}

	bool SoundComponent::IsPaused()
	{
		return sound->IsPaused();
	}

	bool SoundComponent::IsMuted()
	{
		return sound->IsMute();
	}

	void SoundComponent::Pause(bool pause)
	{
		sound->SetPaused(pause);
	}

	void SoundComponent::Mute(bool mute)
	{
		sound->SetMute(mute);
	}

	void SoundComponent::Type::set(SoundType value)
	{
		sound->SetType((thomas::SoundType)value);
	}

	SoundComponent::SoundType SoundComponent::Type::get()
	{
		return (SoundType)sound->GetType();
	}

	void SoundComponent::Clip::set(AudioClip^ value)
	{
		m_clip = value;
		if (value)
			sound->SetClip((thomas::resource::AudioClip*)value->m_nativePtr);
		else
			sound->SetClip(nullptr);
	}

	AudioClip^ SoundComponent::Clip::get() 
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

	void SoundComponent::Looping::set(bool value) 
	{ 
		sound->SetLooping(value); 
	}

	bool SoundComponent::Looping::get() 
	{ 
		return sound->IsLooping(); 
	}

	void SoundComponent::Is3D::set(bool value)
	{
		sound->Set3D(value);
	}

	bool SoundComponent::Is3D::get()
	{
		return sound->Is3D();
	}

	void SoundComponent::MinDistance::set(float value)
	{
		if (value <= 0.f)
		{
			value = 0.f;
		}

		sound->Set3DMinDistance(value);
	}

	float SoundComponent::MinDistance::get()
	{
		return sound->Get3DMinDistance();
	}

	void SoundComponent::MaxDistance::set(float value)
	{
		sound->Set3DMaxDistance(value);
	}

	float SoundComponent::MaxDistance::get()
	{
		return sound->Get3DMaxDistance();
	}

	void SoundComponent::SpreadAngle::set(float value)
	{
		if (value <= 0.f)
		{
			value = 0.f;
		}
		else if (value >= 360.f)
		{
			value = 360.f;
		}

		sound->Set3DSpreadAngle(value);
	}

	float SoundComponent::SpreadAngle::get()
	{
		return sound->Get3DSpreadAngle();
	}
}