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

			void OnChanged();

		private:

			std::vector<Texture2D*> m_textures;
			std::vector<unsigned> m_textureRefCount;

			DXGI_FORMAT m_format;
		};
	}
}