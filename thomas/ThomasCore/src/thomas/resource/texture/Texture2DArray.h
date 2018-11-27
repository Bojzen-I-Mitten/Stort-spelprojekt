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
			Texture2DArray(unsigned width, unsigned height, DXGI_FORMAT format, unsigned nrOfTextures = 1u, bool isDepthTexture = false);
			~Texture2DArray();

			void UnloadTextures();
			void DeRefTexture(unsigned i);
			
			unsigned AddTexture(Texture2D* tex);

			void OnChanged();
			void UpdateTextures();
		private:
			std::vector<Texture2D*> m_referenceTextures;//stores a comparison address
			std::vector<Texture2D*> m_textures;
			std::vector<unsigned> m_textureRefCount;

			DXGI_FORMAT m_format;
			unsigned m_capacity;
		};
	}
}