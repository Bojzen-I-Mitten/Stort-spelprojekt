#include "Texture2DArray.h"
#include "../../Common.h"
#include "../../utils/D3D.h"

namespace thomas {
	namespace resource 
	{
		Texture2DArray::Texture2DArray(unsigned width, unsigned height, DXGI_FORMAT format, unsigned nrOfTextures, bool isDepthTexture)
		{
			m_width = width;
			m_height = height;
			m_format = format;
			m_capacity = nrOfTextures;

			m_srv = nullptr;
			m_resource = nullptr;

			ID3D11Texture2D *textureInterface = nullptr;
			if (isDepthTexture)
			{
				utils::D3D::Instance()->CreateDepthStencilTextureArray(m_width, m_height, textureInterface, m_srv, nrOfTextures);
				m_resource = textureInterface;
			}
			else
			{
				utils::D3D::Instance()->CreateTextureArray(nullptr, m_width, m_height, m_capacity, m_format, textureInterface, m_srv, false, 1);
				m_resource = textureInterface;	// ;|
				AddTexture(Texture2D::GetWhiteTexture());
			}
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
			m_referenceTextures.clear();
			m_textureRefCount.clear();

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
				m_referenceTextures.erase(m_referenceTextures.begin() + i, m_referenceTextures.begin() + i + 1);
				m_textureRefCount.erase(m_textureRefCount.begin() + i, m_textureRefCount.begin() + i + 1);

				//onchanged
			}
		}

		unsigned Texture2DArray::AddTexture(Texture2D* tex)
		{
			unsigned i = 0;
			for (; i < m_referenceTextures.size(); ++i)
			{
				if (m_referenceTextures[i] == tex)//resource manager makes sure the address is unique per texture
				{
					m_textureRefCount[i]++;
					
					return i;
				}
			}
			Texture2D* texCpy = new Texture2D(tex->GetRawBGRAPixels(), tex->GetWidth(), tex->GetHeight(), false);
			//resize
			//tex->ChangeFormat(m_format);
			texCpy->Resize(m_width, m_height);

			m_referenceTextures.push_back(tex);
			m_textures.push_back(texCpy);
			m_textureRefCount.push_back(1);

			//update
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
			m_capacity = m_textures.size();
			
			ID3D11Texture2D *textureInterface = nullptr;
			utils::D3D::Instance()->CreateTextureArray((void**)initData.data(), m_width, m_height, (int)initData.size(), m_format, textureInterface, m_srv, true, 1);
			m_resource = textureInterface;
		}

		void Texture2DArray::UpdateTextures()
		{
			
			unsigned size = m_width * m_height;
			unsigned arraySize = m_textures.size() * size;
			std::vector<byte*> initData(arraySize);

			for (unsigned i = 0; i < arraySize; ++size)
			{
				initData[i] = m_textures[i]->GetRawBGRAPixels();
			}

			utils::D3D::Instance()->GetDeviceContextImmediate()->UpdateSubresource(m_resource, 0, NULL, &initData, 4 * m_width, 4 * m_width * m_height);
		}
	}
}