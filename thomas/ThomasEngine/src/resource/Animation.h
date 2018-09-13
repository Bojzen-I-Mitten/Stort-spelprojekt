#pragma once
#pragma unmanaged
#include <thomas\resource\Animation.h>
#include <memory>
#pragma managed
#include "Resource.h"

namespace ThomasEngine
{

	[DataContractAttribute]
	public ref class Animation : public Resource
	{
	internal:
		Animation(thomas::resource::Animation* nativePtr);
	public:
		Animation(String^ path);

		virtual ~Animation();
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);
	};
}
