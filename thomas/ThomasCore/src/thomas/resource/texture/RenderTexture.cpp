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

			utils::D3D::Instance()->CreateRenderTarget(width, height, textureInterface, m_rtv, m_srv, false);
			

			m_resource = textureInterface;
			data = new DirectX::ScratchImage();
		}

		void RenderTexture::WriteCanvas(graphics::GUI::Canvas * canvas)
		{

			utils::D3D::Instance()->GetDeviceContext()->ClearRenderTargetView(m_rtv, Color(1.0f, 1.0f, 1.0f));
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);

			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(1, &m_rtv, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetDepthStencilState(0, 0);
			canvas->Render();
		}


	}
}
