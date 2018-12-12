#include "Texture2D.h"
#include "../../utils/D3D.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
#include "../ResourceManager.h"
#include "../../utils/ProfileManager.h"
namespace thomas
{
	namespace resource
	{
		void Texture2D::LoadTextureFromFile(std::string path)
		{
			if (utils::D3D::Instance()->LoadTextureFromFile(path, m_resource, m_srv))
			{
				ID3D11Texture2D *textureInterface;

				m_resource->QueryInterface<ID3D11Texture2D>(&textureInterface);


				D3D11_TEXTURE2D_DESC desc;

				textureInterface->GetDesc(&desc);
				m_mipmapCount = desc.MipLevels;
				m_width = desc.Width;
				m_height = desc.Height;

				textureInterface->Release();
				data = new DirectX::ScratchImage();
#ifdef BENCHMARK
				float vramUsage = utils::profiling::ProfileManager::lastTextureVramUsage();
				utils::profiling::ProfileManager::addTextureData(path, vramUsage);
#endif
			}
		}

		Texture2D::Texture2D(std::string path, int width, int height, bool mipMap, bool bindDepth): Texture2D(path, nullptr, width, height, mipMap, bindDepth)
		{
		}

		Texture2D::Texture2D(std::string path, void * initData, int width, int height, bool mipMap, bool bindDepth)
		{
			m_width = width;
			m_height = height;
			m_mipmapCount = 1;
			m_mipMap = mipMap;

			ID3D11Texture2D *textureInterface = nullptr;

			if (bindDepth)
			{
				utils::D3D::Instance()->CreateDepthStencilTexture(width, height, textureInterface, m_srv);

				static const char c_szName[] = "Stencil";
				textureInterface->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
			}
			else
			{
				utils::D3D::Instance()->CreateTexture(initData, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, textureInterface, m_srv, mipMap, 1);

				static const char c_szName[] = "Texture";
				textureInterface->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName) - 1, c_szName);
			}

			m_resource = textureInterface;
			data = new DirectX::ScratchImage();

#ifdef BENCHMARK
			float vramUsage = utils::profiling::ProfileManager::lastTextureVramUsage();
			utils::profiling::ProfileManager::addTextureData(path, vramUsage);
#endif
		}

		Texture2D::Texture2D(std::string path) : Texture(path)
		{
			LoadTextureFromFile(path);
		}

		void Texture2D::OnChanged()
		{
			SAFE_RELEASE(m_srv);
			SAFE_RELEASE(m_resource);
			LoadTextureFromFile(m_path);
		}

		std::vector<math::Color> Texture2D::GetPixels()
		{
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextImmediate(), m_resource, *data);

			std::vector<math::Color> pixels;
			DirectX::PackedVector::XMUBYTEN4* rawPixels = (DirectX::PackedVector::XMUBYTEN4*)data->GetPixels();

			for (int i = 0; i < data->GetPixelsSize() / 4; i++)
			{
				pixels.push_back(math::Color(rawPixels[i].v));
			}

			return pixels;
		}

		/*byte * Texture2D::GetRawRGBAPixels()
		{
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextDeferred(), m_resource, *data);
			
			return data->GetPixels();
		}*/

		byte * Texture2D::GetRawBGRAPixels()
		{
			DirectX::ScratchImage firstData;
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextImmediate(), m_resource, firstData);
			hr = DirectX::Convert(*firstData.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *data);
			firstData.Release();
			
			return data->GetPixels();
		}

		/*bool Texture2D::ChangeFormat(DXGI_FORMAT format)
		{
			DirectX::ScratchImage firstData;
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextDeferred(), m_resource, firstData);

			hr = DirectX::Convert(*firstData.GetImage(0, 0, 0), format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *data);



			firstData.Release();
			if (FAILED(hr))
			{
				LOG("Failed to convert image");
				return false;
			}
			
			return true;
		}
		*/
		bool Texture2D::Resize(int width, int height)
		{
			DirectX::ScratchImage* resizedImage = new DirectX::ScratchImage();
			
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextImmediate(), m_resource, *data);
			hr = DirectX::Resize(*data->GetImage(0, 0, 0), width, height, DirectX::TEX_FILTER_DEFAULT, *resizedImage);
			if (FAILED(hr))
			{
				LOG("Failed to resize image");
				return false;
			}
			
			//replace data
			SAFE_RELEASE(m_resource);
			hr = DirectX::CreateTexture(utils::D3D::Instance()->GetDevice(), resizedImage->GetImage(0, 0, 0), 1, resizedImage->GetMetadata(), &m_resource);
			
			
			delete data;

			data = resizedImage;

			return true;
		}

		Texture2D * Texture2D::GetBlackTexture()
		{
			return ResourceManager::GetBlackTexture();
		}

		Texture2D * Texture2D::GetWhiteTexture()
		{
			return ResourceManager::GetWhiteTexture();
		}

		Texture2D * Texture2D::GetNormalTexture()
		{
			return ResourceManager::GetNormalTexture();
		}

		Texture2D::~Texture2D()
		{
			
		}
	}
}
