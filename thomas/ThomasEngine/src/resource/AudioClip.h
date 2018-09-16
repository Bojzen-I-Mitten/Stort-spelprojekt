#pragma once

#include "Resource.h"

namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class AudioClip : public Resource
	{
	public:
		AudioClip(System::String^ path);

		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);
	};
}
