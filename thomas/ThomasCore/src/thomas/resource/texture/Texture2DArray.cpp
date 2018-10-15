#include "Texture2DArray.h"
#include "../../Common.h"
#include "../../utils/d3d.h"

namespace thomas {
	namespace resource {



		Texture2DArray::Texture2DArray(int width, int height, DXGI_FORMAT format)
		{
			m_width = width;
			m_height = height;
			m_format = format;

			m_srv = nullptr;
			m_resource = nullptr;
		}

		unsigned Texture2DArray::AddTexture(Texture2D* tex)
		{

			unsigned i = 0;
			for (; i < m_textures.size(); ++i)
			{
				if (m_textures[i] == tex)//resource manager makes sure the address is unique per texture
					return i;
			}
			Texture2D* texCpy = new Texture2D(tex);
			//resize
			//tex->ChangeFormat(m_format);
			texCpy->Resize(m_width, m_height);

			m_textures.push_back(texCpy);

			//update
			OnChanged();

			return i;
		}


		void Texture2DArray::OnChanged()
		{
			std::vector<byte*> initData;
			for (Texture2D* tex : m_textures)
			{
				initData.push_back(tex->GetRawRGBAPixels());
			}

			ID3D11Texture2D *textureInterface = nullptr;
			utils::D3D::Instance()->CreateTextureArray((void**)initData.data(), m_width, m_height, initData.size(), m_format, textureInterface, m_srv, true, 1);
			m_resource = textureInterface;
		}
	}
}