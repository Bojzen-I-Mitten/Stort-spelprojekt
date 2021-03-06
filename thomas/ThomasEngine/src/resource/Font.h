#pragma once
#pragma unmanaged

#pragma managed
#include "Resource.h"

namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class Font : public Resource
	{
	public:
		Font(String^ path);
		virtual ~Font();

		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);
	};
}