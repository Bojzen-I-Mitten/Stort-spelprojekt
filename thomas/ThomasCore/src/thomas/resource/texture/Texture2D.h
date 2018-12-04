#pragma once

#include "Texture.h"
#include "../../utils/Math.h"
#include "../../utils/d3d.h"
#include <DirectXTex.h>

namespace thomas
{
	namespace resource
	{
		class Texture2D : public Texture
		{
		private:
			void LoadTextureFromFile(std::string path);
		public:
			Texture2D(int width, int height, bool mipMap = false, bool bindDepth = false);
			Texture2D(void* initData, int width, int height, bool mipMap = false, bool bindDepth = false);
			Texture2D(std::string path);

			void OnChanged();
			std::vector<math::Color> GetPixels();
			//byte* GetRawRGBAPixels();
			byte* GetRawBGRAPixels();

			//bool ChangeFormat(DXGI_FORMAT format);
			bool Resize(int width, int height);

			static Texture2D* GetBlackTexture();
			static Texture2D* GetWhiteTexture();
			static Texture2D* GetNormalTexture();

		private:
			DirectX::ScratchImage* data;
			int m_mipmapCount;
			bool m_mipMap;
		};
	}
}