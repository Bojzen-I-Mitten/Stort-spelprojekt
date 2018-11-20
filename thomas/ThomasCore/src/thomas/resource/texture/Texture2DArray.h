#pragma once
#pragma once

#include "Texture2D.h"
#include <DirectXTex.h>

namespace thomas
{
	namespace resource
	{
		class Texture2DArray : public Texture
		{
		private:
			
		public:
			Texture2DArray(int width, int height, DXGI_FORMAT format);
			~Texture2DArray();

			void UnloadTextures();
			void DeRefTexture(unsigned i);
			
			unsigned AddTexture(Texture2D* tex);
			unsigned AddTextureReference(Texture2D*& tex);//does not create a new copy nor resize the texture

			void OnChanged();
			void UpdateTextures();
		private:
			std::vector<Texture2D*> m_referenceTextures;//stores a comparison address
			std::vector<Texture2D*> m_textures;
			std::vector<unsigned> m_textureRefCount;

			DXGI_FORMAT m_format;
		};
	}
}