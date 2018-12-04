#include "ResourceManager.h"
#include "../utils/Math.h"
#include "../utils/d3d.h"
#include <DirectXTex.h>
#include "texture/Texture2D.h"
#include "texture/TextureCube.h"

namespace thomas
{
	namespace resource
	{
	Texture2D* ResourceManager::s_blackTexture;
	Texture2D* ResourceManager::s_whiteTexture;
	Texture2D* ResourceManager::s_normalTexture;
	TextureCube* ResourceManager::s_cubeMap;

		void ResourceManager::Init()
		{
			DirectX::PackedVector::XMUBYTEN4 colors[1];
			std::fill(std::begin(colors), std::end(colors), math::Color(0, 0, 0, 1).RGBA());
			s_blackTexture = new Texture2D((void*)&colors, 1, 1, false);
			s_blackTexture->SetPath("Black Texture");
			std::fill(std::begin(colors), std::end(colors), math::Color(0.5f, 0.5f, 1, 1).RGBA());
			s_normalTexture = new Texture2D((void*)&colors, 1, 1, false);
			s_blackTexture->SetPath("Normal Texture");
			std::fill(std::begin(colors), std::end(colors), math::Color(1, 1, 1, 1).RGBA());
			s_whiteTexture = new Texture2D((void*)&colors, 1, 1, false);
			s_blackTexture->SetPath("White Texture");


			s_cubeMap = new TextureCube(0xffffffff);
		}
		void ResourceManager::Destroy()
		{
			delete s_blackTexture;
			delete s_whiteTexture;
			delete s_normalTexture;
			delete s_cubeMap;
		}


		Texture2D * ResourceManager::GetBlackTexture()
		{
			return s_blackTexture;
		}

		Texture2D * ResourceManager::GetWhiteTexture()
		{
			return s_whiteTexture;
		}

		Texture2D * ResourceManager::GetNormalTexture()
		{
			return s_normalTexture;
		}
		TextureCube * ResourceManager::GetCubeDefault()
		{
			return s_cubeMap;
		}
	}
}