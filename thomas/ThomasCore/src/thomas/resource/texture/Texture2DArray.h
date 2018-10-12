#pragma once
#pragma once

#include "Texture2D.h"
namespace thomas
{
	namespace resource
	{
		class Texture2DArray : Texture
		{
		private:
			
		public:
			Texture2DArray(int width, int height, int arraySize);
			
			bool AddTexture(Texture2D tex);


			void OnChanged();

		private:
			DirectX::ScratchImage* data = nullptr;
			int m_mipmapCount;
			bool m_linear;
			bool m_mipMap;

			std::vector<Texture2D> m_textures;
			
		};
	}
}