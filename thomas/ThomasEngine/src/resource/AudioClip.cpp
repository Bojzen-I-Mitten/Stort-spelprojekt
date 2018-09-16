#pragma unmanaged
#include <thomas\resource\AudioClip.h>
#pragma managed

#include "AudioClip.h"

namespace ThomasEngine
{
	AudioClip::AudioClip(System::String^ path) : Resource(path, new thomas::resource::AudioClip(Utility::ConvertString(path))) {};

	void AudioClip::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = new thomas::resource::AudioClip(Utility::ConvertString(m_path));
	}

}