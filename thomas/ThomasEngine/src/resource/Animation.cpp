#include "Animation.h"
#include "../object/GameObject.h"

namespace ThomasEngine
{
	Animation::Animation(thomas::resource::Animation* nativePtr)
		: Resource(Utility::ConvertString(nativePtr->GetPath()), nativePtr)
	{}
	Animation::Animation(String^ path) :
		Resource(path, new thomas::resource::Animation(Utility::ConvertString(path)))
	{}
	thomas::resource::Animation * Animation::Native()
	{
		return (thomas::resource::Animation*)m_nativePtr;
	}

	Animation::~Animation()
	{
		if (m_nativePtr)
			delete m_nativePtr;
	}
	void Animation::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = new thomas::resource::Animation(Utility::ConvertString(Path));
	}
}