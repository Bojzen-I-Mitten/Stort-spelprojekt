#pragma once
#pragma unmanaged
#include <thomas\resource\Font.h>
#pragma managed
#include "Font.h"

namespace ThomasEngine
{
	Font::Font(System::String^ path) : Resource(path, new thomas::resource::Font(Utility::ConvertString(path))) {}

	void Font::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = new thomas::resource::Font(Utility::ConvertString(m_path));
	}

}
