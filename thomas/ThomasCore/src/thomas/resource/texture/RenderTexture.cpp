#include "RenderTexture.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
#include "../../graphics/GUI/Canvas.h"
namespace thomas
{
	namespace resource
	{


		RenderTexture::RenderTexture(int width, int height)
		{
			m_width = width;
			m_height = height;

			ID3D11Texture2D *textureInterface = nullptr;

			utils::D3D::Instance()->CreateRenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, textureInterface, m_srv, m_rtv);
			

			m_resource = textureInterface;
			data = new DirectX::ScratchImage();
		}

		RenderTexture::~RenderTexture()
		{
			SAFE_RELEASE(m_rtv);
		}

		void RenderTexture::WriteCanvas(graphics::GUI::Canvas * canvas)
		{
			Bind();
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, canvas->GetViewport().Get11());
			canvas->Render();

			utils::D3D::Instance()->GetDeviceContext()->GenerateMips(m_srv);
		}

		void RenderTexture::Bind()
		{
			utils::D3D::Instance()->GetDeviceContext()->ClearRenderTargetView(m_rtv, Color(1.0f, 1.0f, 1.0f, 0.0f));
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);

			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(1, &m_rtv, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetDepthStencilState(0, 0);
		}


	}
}