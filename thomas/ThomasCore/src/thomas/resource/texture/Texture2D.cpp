#include "Texture2D.h"
#include "../../Common.h"
#include "../../ThomasCore.h"

namespace thomas
{
	namespace resource
	{
		Texture2D* Texture2D::s_blackTexture;
		Texture2D* Texture2D::s_whiteTexture;
		Texture2D* Texture2D::s_normalTexture;

		void Texture2D::Init()
		{
			DirectX::PackedVector::XMUBYTEN4 colors[16];
			std::fill(std::begin(colors), std::end(colors), math::Color(0, 0, 0, 1).RGBA());
			s_blackTexture = new Texture2D((void*)&colors, 4, 4, false);
			s_blackTexture->m_path = "Black Texture";
			std::fill(std::begin(colors), std::end(colors), math::Color(0.5f, 0.5f, 1, 1).RGBA());
			s_normalTexture = new Texture2D((void*)&colors, 4, 4, false);
			s_normalTexture->m_path = "Normal Texture";
			std::fill(std::begin(colors), std::end(colors), math::Color(1, 1, 1, 1).RGBA());
			s_whiteTexture = new Texture2D((void*)&colors, 4, 4, false);
			s_whiteTexture->m_path = "White Texture";
		}
		void Texture2D::Destroy()
		{
			delete s_blackTexture;
			delete s_whiteTexture;
			delete s_normalTexture;
		}

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
			}
		}

		Texture2D::Texture2D(int width, int height, bool mipMap, DXGI_FORMAT format, bool bindDepth): Texture2D(nullptr, width, height, mipMap, format, bindDepth)
		{
		}

		Texture2D::Texture2D(void * initData, int width, int height, bool mipMap, DXGI_FORMAT format, bool bindDepth)
		{
			m_width = width;
			m_height = height;
			m_mipmapCount = 1;
			m_mipMap = mipMap;

			ID3D11Texture2D *textureInterface = nullptr;
			utils::D3D::Instance()->CreateTexture(initData, width, height, format, textureInterface, m_srv, mipMap, 1, bindDepth);
			m_resource = textureInterface;
			data = new DirectX::ScratchImage();
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
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), m_resource, *data);

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
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), m_resource, *data);
			
			return data->GetPixels();
		}*/

		byte * Texture2D::GetRawBGRAPixels()
		{
			DirectX::ScratchImage firstData;
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), m_resource, firstData);
			hr = DirectX::Convert(*firstData.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *data);
			firstData.Release();

			return data->GetPixels();
		}

		/*bool Texture2D::ChangeFormat(DXGI_FORMAT format)
		{
			DirectX::ScratchImage firstData;
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), m_resource, firstData);

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
			
			HRESULT hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), m_resource, *data);
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
			return s_blackTexture;
		}

		Texture2D * Texture2D::GetWhiteTexture()
		{
			return s_whiteTexture;
		}

		Texture2D * Texture2D::GetNormalTexture()
		{
			return s_normalTexture;
		}


	}
}
