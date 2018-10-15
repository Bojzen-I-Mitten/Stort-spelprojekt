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
			
			unsigned AddTexture(Texture2D* tex);

			void OnChanged();

		private:
			int m_mipmapCount;
			bool m_linear;
			bool m_mipMap;

			std::vector<Texture2D*> m_textures;
			
			DXGI_FORMAT m_format;
		};
	}
}