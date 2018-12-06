#include "RenderTexture.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
#include "../../graphics/GUI/Canvas.h"
namespace thomas
{
	namespace resource
	{


		RenderTexture::RenderTexture(int width, int height) : m_commandList(nullptr)
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
			SAFE_RELEASE(m_commandList);
			SAFE_RELEASE(m_rtv);
		}

		void RenderTexture::WriteCanvas(graphics::GUI::Canvas * canvas)
		{
			utils::D3D::Instance()->ResetCommandList(m_commandList);
			Bind();
			utils::D3D::Instance()->GetDeviceContextDeferred()->RSSetViewports(1, canvas->GetViewport().Get11());
			canvas->Render();

			utils::D3D::Instance()->GetDeviceContextDeferred()->GenerateMips(m_srv);
			utils::D3D::Instance()->FinishCommandList(m_commandList);
			utils::D3D::Instance()->ExecuteCommandList(m_commandList);
		}

		void RenderTexture::Bind()
		{
			utils::D3D::Instance()->GetDeviceContextDeferred()->ClearRenderTargetView(m_rtv, Color(1.0f, 1.0f, 1.0f, 0.0f));
			utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(0, NULL, NULL);

			utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(1, &m_rtv, NULL);
		}
	}
}
