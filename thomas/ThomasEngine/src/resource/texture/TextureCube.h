#pragma once
#pragma unmanaged
#include <thomas\resource\texture\TextureCube.h>
#pragma managed

#include "Texture.h"
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class TextureCube : public Texture
	{
	public:

	internal:
		TextureCube(String^ path) : Texture(path, new thomas::resource::TextureCube(Utility::ConvertString(path))) {};
		TextureCube(thomas::resource::TextureCube* nativePtr) : Texture(Utility::ConvertString(nativePtr->GetPath()), nativePtr) {};

	public:
		
		
	};
}
