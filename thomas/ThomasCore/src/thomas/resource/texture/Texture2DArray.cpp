#include "Texture2DArray.h"
#include "../../Common.h"

namespace thomas {
	namespace resource {



		Texture2DArray::Texture2DArray(int width, int height, int arraySize)
		{

		}

		bool Texture2DArray::AddTexture(Texture2D tex)
		{
			if (tex.GetHeight() != m_height && tex.GetWidth() != m_width)
			{
				LOG("Texture is of wrong size");
				return false;
			}
			return true;
		}

		void Texture2DArray::OnChanged()
		{
			for (Texture2D tex : m_textures)
			{
				tex.OnChanged();
				byte* temp = tex.GetRawBGRAPixels();
			}
		}
	}
}