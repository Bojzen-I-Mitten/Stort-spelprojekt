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