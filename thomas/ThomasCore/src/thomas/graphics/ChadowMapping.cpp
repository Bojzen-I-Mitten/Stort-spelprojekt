#include "ChadowMapping.h"
#include "../object/component/Camera.h"
#include "../object/component/Transform.h"
#include "../resource/Shader.h"
#include "../resource/Material.h"
#include "../resource/texture/Texture2DArray.h"
#include "Mesh.h"
#include "render/Frame.h"
#include "RenderConstants.h"

namespace thomas
{
	namespace graphics
	{
		std::unique_ptr<resource::Material> ShadowMap::s_material;
		std::unique_ptr<resource::Shader> ShadowMap::s_shader;
		D3D11_VIEWPORT ShadowMap::s_viewPort;

		void ShadowMap::InitStatics(unsigned size)
		{
			s_shader = resource::Shader::CreateShader("../Data/FXIncludes/ShadowPassShader.fx");
			s_material = std::make_unique<resource::Material>(s_shader.get());
			
			s_viewPort.Height = size;
			s_viewPort.Width = size;
			s_viewPort.TopLeftY = 0.0f;
			s_viewPort.TopLeftX = 0.0f;
			s_viewPort.MinDepth = 0.0f;
			s_viewPort.MaxDepth = 1.0f;
		}

		ShadowMap::ShadowMap()
		{
			m_matrixView = math::Matrix::CreateLookAt(math::Vector3::Up, math::Vector3::Zero, math::Vector3::Up);
			m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-20, 20, -20, 20, -10, 40);//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
			m_matrixVP = m_matrixView * m_matrixProj;
			m_depthStencilView = nullptr;
		}

		ShadowMap::~ShadowMap()
		{
			SAFE_RELEASE(m_depthStencilView);
		}

		void ShadowMap::UpdateShadowBox(object::component::Transform* lightTransform, object::component::Camera* camera)
		{
			//https://www.gamedev.net/forums/topic/505893-orthographic-projection-for-shadow-mapping/
			if (camera != nullptr)
			{
				/*math::Vector3 corners[8];
				camera->GetFrustrum().GetCorners(corners);

				float distance = 30.0f;

				for (unsigned i = 0; i < 4; ++i)
				{
					math::Vector3 dir = corners[4 + i] - corners[0 + i];
					dir.Normalize();
					corners[4 + i] = corners[0 + i] + dir * distance;
				}
				
				
				math::Vector3 frustumCenter = math::Vector3(0, 0, 0);


				for (unsigned i = 0; i < 8; ++i)
				{
					frustumCenter += corners[i];
					//math::Vector3::Transform(corners[i], m_matrixView);//transform corner into lightspace
				}*/
				//frustumCenter /= 8;
				
				math::Vector3 frustumCenter = camera->GetPosition() + camera->GetDirection() * 10.0f;
				//const float nearClipOffset = 20.0f;
				m_matrixView = math::Matrix::CreateLookAt(frustumCenter + lightTransform->Forward() * 10.0f, frustumCenter, math::Vector3::Up); //lightTransform->GetWorldMatrix();
			
				//for (unsigned i = 0; i < 8; ++i)
					//corners[i] = math::Vector3::Transform(corners[i], m_matrixView);

				//find extreme points
				/*math::Vector3 maxes = corners[0];
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
				}*/
				//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(frustumCenter.x -10, frustumCenter.x + 10, frustumCenter.y -10, frustumCenter.y + 10, frustumCenter.z - 25, frustumCenter.z + 60);
				//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, maxes.z, mins.z);
			}
			else
			{
				m_matrixView = math::Matrix::CreateLookAt(lightTransform->Forward() * 20/* (camera->GetFar() + nearClipOffset)*/, math::Vector3::Zero, math::Vector3::Up); //lightTransform->GetWorldMatrix();
				//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-10, 10, -10, 10, -10, 30);
			}
			
			
			//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, -maxes.z - nearClipOffset, -mins.z);
			
			

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
			if (m_depthStencilView == nullptr)
				return;

			ID3D11ShaderResourceView* const s_nullSRV[8] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->PSSetShaderResources(0, 8, s_nullSRV);

			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);
			utils::D3D::Instance()->GetDeviceContext()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, nullptr, m_depthStencilView);
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, &s_viewPort);

			s_material->Bind();
			s_material->SetMatrix("lightMatrixVP", m_matrixVP.Transpose());
			s_material->ApplyProperty("lightMatrixVP");
			
		}

		void ShadowMap::SetShadowMapDepthStencilView(ID3D11DepthStencilView * dsv)
		{
			m_depthStencilView = dsv;
		}

		ID3D11DepthStencilView * ShadowMap::GetShadowMapDepthStencilView()
		{
			return m_depthStencilView;
		}

		math::Matrix ShadowMap::GetVPMat()
		{
			return m_matrixVP.Transpose();
		}

		
	}
}