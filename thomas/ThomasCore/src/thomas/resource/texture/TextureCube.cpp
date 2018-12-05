#include "TextureCube.h"
#include "../../utils/d3d.h"
#include "../../Common.h"

#include "../../ThomasCore.h"
namespace thomas
{
	namespace resource
	{
		TextureCube::TextureCube(std::string path) : Texture(path)
		{
			LoadTextureCubeFromFile(path);
		}

		TextureCube::TextureCube(uint32_t color)
		{

			m_width = 1;
			m_height = 1;

			ID3D11Texture2D *textureInterface = nullptr;
			void* data[6] = { &color, &color, &color, &color, &color, &color };
			utils::D3D::Instance()->CreateTextureCubeMap(data, 1, DXGI_FORMAT_R8G8B8A8_UNORM, textureInterface, m_srv, 0, 1);
		}

		TextureCube::~TextureCube()
		{

		}

		void TextureCube::LoadTextureCubeFromFile(std::string path)
		{
			ID3D11Resource* tempResource;
			ID3D11ShaderResourceView* tempSRV;
			if (utils::D3D::Instance()->LoadCubeTextureFromFile(path, tempResource, tempSRV))
			{
				SAFE_RELEASE(m_resource);
				SAFE_RELEASE(m_srv);
				m_resource = tempResource;
				m_srv = tempSRV;
				
			}
			else
			{
				LOG("Invalid skymap texture at path:" + path);
			}
		}
	}
}