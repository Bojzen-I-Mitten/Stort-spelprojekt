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

		Texture2DArray::~Texture2DArray()
		{
			UnloadTextures();
		}

		void Texture2DArray::UnloadTextures()
		{
			for (Texture2D* tex : m_textures)
				SAFE_DELETE(tex);
			m_textures.clear();
		}

		void Texture2DArray::DeRefTexture(unsigned i)
		{
			if (m_textureRefCount.size() <= i)//Invalid deref
			{
				return;
			}

			m_textureRefCount[i]--;
			if (m_textureRefCount[i] == 0)
			{
				SAFE_DELETE(m_textures[i]);
				m_textures.erase(m_textures.begin() + i, m_textures.begin() + i + 1);
				m_textureRefCount.erase(m_textureRefCount.begin() + i, m_textureRefCount.begin() + i + 1);
			}
		}

		unsigned Texture2DArray::AddTexture(Texture2D* tex)
		{
			unsigned i = 0;
			for (; i < m_textures.size(); ++i)
			{
				if (m_textures[i] == tex)//resource manager makes sure the address is unique per texture
				{
					m_textureRefCount[i]++;
					
					return i;
				}
			}
			Texture2D* texCpy = new Texture2D(tex->GetRawBGRAPixels(), tex->GetWidth(), tex->GetHeight(), false);
			//resize
			//tex->ChangeFormat(m_format);
			texCpy->Resize(m_width, m_height);

			m_textures.push_back(texCpy);
			m_textureRefCount.push_back(1);

			//update
			OnChanged();

			return i;
		}

		unsigned Texture2DArray::AddTextureReference(Texture2D *& tex)
		{
			if (tex->GetWidth() != m_width)
				return -1;
			if (tex->GetHeight() != m_height)
				return -1;


			unsigned i = 0;
			for (; i < m_textures.size(); ++i)
			{
				if (m_textures[i] == tex)//resource manager makes sure the address is unique per texture
				{
					m_textureRefCount[i]++;

					return i;
				}
			}
			m_textures.push_back(tex);
			OnChanged();

			return i;
		}


		void Texture2DArray::OnChanged()
		{
			SAFE_RELEASE(m_resource);
			SAFE_RELEASE(m_srv);

			std::vector<byte*> initData;
			for (Texture2D* tex : m_textures)
			{
				initData.push_back(tex->GetRawBGRAPixels());
			}

			ID3D11Texture2D *textureInterface = nullptr;
			utils::D3D::Instance()->CreateTextureArray((void**)initData.data(), m_width, m_height, initData.size(), m_format, textureInterface, m_srv, true, 1);
			m_resource = textureInterface;
		}
		void Texture2DArray::UpdateTextures()
		{
			
			unsigned size = m_width * m_height;
			unsigned arraySize = m_textures.size() *size;
			std::vector<byte*> initData(arraySize);

			for (unsigned i = 0; i < arraySize; ++size)
			{
				initData[i] = m_textures[i]->GetRawBGRAPixels();
			}

			utils::D3D::Instance()->GetDeviceContext()->UpdateSubresource(m_resource, 0, NULL, &initData, 4 * m_width, 4 * m_width * m_height);
		}
	}
}