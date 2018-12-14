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
			Texture2DArray(unsigned dim, DXGI_FORMAT format, unsigned nrOfTextures = 1u, bool isDepthTexture = false);
			virtual ~Texture2DArray();

			void UnloadTextures();
			void DeRefTexture(unsigned i);
			
			unsigned AddTexture(Texture2D* tex);

			void OnChanged();
		private:
			std::vector<Texture2D*> m_referenceTextures;//stores a comparison address
			std::vector<Texture2D*> m_textures;
			std::vector<unsigned> m_textureRefCount;

			DXGI_FORMAT m_format;
			unsigned m_capacity;
		};
	}
}