#include "Texture.h"
#include "TextureCube.h"

namespace ThomasEngine
{
	Texture::Texture(String^ path) : 
		Resource(path, new thomas::resource::Texture(Utility::ConvertString(path))) 
	{}
	Texture::Texture(String^ path, thomas::resource::Texture* nativePtr) : 
		Resource(path, nativePtr) 
	{}

	Texture::~Texture()
	{
		delete m_nativePtr;
	}




	TextureCube::~TextureCube()
	{

	}

}