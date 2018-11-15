#include "ChadowMapping.h"
#include "../object/component/Camera.h"
#include "../object/component/Transform.h"
#include "../resource/Shader.h"
#include "../resource/Material.h"
#include "../resource/texture/Texture2D.h"
#include "Mesh.h"
#include "render/Frame.h"
#include "RenderConstants.h"

namespace thomas
{
	namespace graphics
	{
		std::unique_ptr<resource::Material> ShadowMap::s_material;
		std::unique_ptr<resource::Shader> ShadowMap::s_shader;
		math::Matrix ShadowMap::s_matrixClamp;
		void ShadowMap::InitStatics()
		{
			s_shader = resource::Shader::CreateShader("../Data/FXIncludes/ShadowPassShader.fx");
			s_material = std::make_unique<resource::Material>(s_shader.get()); 
			s_matrixClamp = math::Matrix(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f);
		}

		ShadowMap::ShadowMap()
		{
			m_depthTexture = std::make_unique<resource::Texture2D>(512, 512, false, true);

			m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-10, 10, -10, 10, -10, 20);//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/



			D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
			depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			HRESULT hr = utils::D3D::Instance()->GetDevice()->CreateDepthStencilView(m_depthTexture->GetResource(), &depthViewDesc, &m_depthStencilView);
			
			m_viewPort.Height = 512;
			m_viewPort.Width = 512;
			m_viewPort.TopLeftY = 0;
			m_viewPort.TopLeftX = 0;
			m_viewPort.MinDepth = 0.0f;
			m_viewPort.MaxDepth = 1.0f;
		}

		ShadowMap::~ShadowMap()
		{

		}

		void ShadowMap::UpdateShadowBox(object::component::Transform* lightTransform, object::component::Camera* camera)
		{
			//https://www.gamedev.net/forums/topic/505893-orthographic-projection-for-shadow-mapping/


			/*math::Vector3 corners[8];
			camera->GetFrustrum().GetCorners(corners);

			math::Vector3 frustumCenter = math::Vector3(0, 0, 0);

			for (unsigned i = 0; i < 8; ++i)
			{
				frustumCenter += corners[i];
				//math::Vector3::Transform(corners[i], m_matrixView);//transform corner into lightspace
			}
			frustumCenter /= 8;

			const float nearClipOffset = 50.0f;
			m_matrixView = math::Matrix::CreateLookAt(lightTransform->Forward() * 25/* (camera->GetFar() + nearClipOffset), math::Vector3::Zero, math::Vector3::Up); //lightTransform->GetWorldMatrix();
			
			for (unsigned i = 0; i < 8; ++i)
				corners[i] = math::Vector3::Transform(corners[i], m_matrixView);

			//find extreme points
			math::Vector3 maxes = corners[0];
			math::Vector3 mins = corners[0];

			for (int i = 0; i < 8; i++)
			{
				if (corners[i].x > maxes.x)
					maxes.x = corners[i].x;
				else if (corners[i].x < mins.x)
					mins.x = corners[i].x;
				if (corners[i].y > maxes.y)
					maxes.y = corners[i].y;
				else if (corners[i].y < mins.y)
					mins.y = corners[i].y;
				if (corners[i].z > maxes.z)
					maxes.z = corners[i].z;
				else if (corners[i].z < mins.z)
					mins.z = corners[i].z;
			}
			*/
			//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, -maxes.z - nearClipOffset, -mins.z);
			m_matrixView = math::Matrix::CreateLookAt(lightTransform->Forward() * 25/* (camera->GetFar() + nearClipOffset)*/, math::Vector3::Zero, math::Vector3::Up); //lightTransform->GetWorldMatrix();
			m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-60, 60, -60, 60, -20, 40);

			m_matrixVP = m_matrixView * m_matrixProj;

			
		}

		void ShadowMap::Draw(render::RenderCommand& renderCommand)
		{
			s_material->SetMatrix(THOMAS_MATRIX_WORLD, renderCommand.worldMatrix.Transpose());
			s_material->ApplyProperty(THOMAS_MATRIX_WORLD);
			s_material->Draw(renderCommand.mesh);
		}

		void ShadowMap::Bind()
		{
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);
			utils::D3D::Instance()->GetDeviceContext()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, nullptr, m_depthStencilView);
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, &m_viewPort);

			s_material->Bind();
			s_material->SetMatrix("lightMatrixVP", m_matrixVP.Transpose());
			s_material->ApplyProperty("lightMatrixVP");
			
		}

		

		resource::Texture2D * ShadowMap::GetShadowMapTexture()
		{
			//s_material->SetMatrix("lightMatrixVP", (m_matrixVP * s_matrixClamp).Transpose());
			//s_material->ApplyProperty("lightMatrixVP");
			return m_depthTexture.get();
		}

		math::Matrix ShadowMap::GetVPMat()
		{
			return (s_matrixClamp * m_matrixVP).Transpose();
		}

		void ShadowMap::BindShadowToMaterial(resource::Material * mat)
		{
			mat->SetMatrix("lightMatrixVP", (s_matrixClamp.Transpose() * m_matrixVP).Transpose());
			//mat->ApplyProperty("lightMatrixVP");

			mat->SetTexture2D("shadowMap", m_depthTexture.get());
			//mat->ApplyProperty("shadowMap");
		}

	}
}