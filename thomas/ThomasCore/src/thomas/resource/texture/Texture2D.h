#pragma once

#include "Texture.h"
#include "../../utils/Math.h"
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
			static void Init();
			static void Destroy();

			Texture2D(int width, int height, bool mipMap);
			Texture2D(void* initData, int width, int height, bool mipMap);
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
			static Texture2D* s_blackTexture;
			static Texture2D* s_whiteTexture;
			static Texture2D* s_normalTexture;
		};
	}
}