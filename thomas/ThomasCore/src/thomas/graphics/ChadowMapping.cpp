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

		void ShadowMap::Destroy()
		{
			s_shader.reset();
			s_material.reset();
		}

		ShadowMap::ShadowMap()
		{
			m_matrixView = math::Matrix::CreateLookAt(math::Vector3::Up, math::Vector3::Zero, math::Vector3::Up);
			m_nrOfCascades = 2;

			m_matrixVP = m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-10, 10, -10, 10, -10, 10);//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
			
			
			_depthStencilView = nullptr;
		}

		ShadowMap::~ShadowMap()
		{
			
		}

		

		void ShadowMap::UpdateShadowBox(object::component::Transform* lightTransform, object::component::Camera* camera)
		{
			//https://www.gamedev.net/forums/topic/505893-orthographic-projection-for-shadow-mapping/
			if (camera != nullptr)
			{
				
				math::Vector3 corners[8];
				camera->GetFrustumCornersRH(corners);

				// Returns 8 corners position of bounding frustum.
				//     Far     Near
				//    0----1  4----5
				//    |    |  |    |
				//    |    |  |    |
				//    3----2  7----6
				
				float nearOffset = 0.0f;
				float cascadeTopology[2] = { 40, 80 };
				for (unsigned c = 0; c < m_nrOfCascades; ++c)
				{
					for (unsigned i = 0; i < 4; ++i)
					{
						math::Vector3 dir = corners[0 + i] - corners[4 + i];
						

						dir.Normalize();
						math::Vector3 temp = corners[4 + i] + dir * cascadeTopology[c];

						corners[0 + i] = temp;
					}
				
				
					math::Vector3 frustumCenter = math::Vector3(0, 0, 0);

				
					for (unsigned i = 0; i < 8; ++i)
					{
						frustumCenter += corners[i];
						//math::Vector3::Transform(corners[i], m_matrixView);//transform corner into lightspace
					}
					frustumCenter /= 8;

					math::Matrix matrixView = math::Matrix::CreateLookAt(frustumCenter + lightTransform->Forward(), frustumCenter, math::Vector3::Up); //lightTransform->GetWorldMatrix();

					//frustumCenter = math::Vector3::Zero;//camera->GetPosition() + camera->GetDirection() * 40.0f;
					//const float nearClipOffset = 20.0f;
				
			
					for (unsigned i = 0; i < 8; ++i)
						corners[i] = math::Vector3::Transform(corners[i], matrixView);

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

					int stop = 0;
					floor
					//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(frustumCenter.x -10, frustumCenter.x + 10, frustumCenter.y -10, frustumCenter.y + 10, frustumCenter.z - 25, frustumCenter.z + 60);
					m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, mins.z, maxes.z);
					m_matrixVP = matrixView * m_matrixProj;
				}
			}
			
			
			
			//m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, -maxes.z - nearClipOffset, -mins.z);
			
			

			

			
		}

		void ShadowMap::Draw(render::RenderCommand& renderCommand)
		{
			s_material->SetMatrix(THOMAS_MATRIX_WORLD, renderCommand.worldMatrix.Transpose());
			s_material->ApplyProperty(THOMAS_MATRIX_WORLD);
			
			if (renderCommand.num_local_prop)
			{
				int temp = 10;
				
				s_material->SetInt("animate", temp);
				renderCommand.local_prop[0].m_apply(renderCommand.local_prop[0], s_material->GetShader());

			}
			else
			{
				int temp = 0;

				s_material->SetInt("animate", temp);
				
			}
			s_material->ApplyProperty("animate");
			s_material->Draw(renderCommand.mesh);
		}

		void ShadowMap::Bind()
		{
			if (_depthStencilView == nullptr)
				return;

			ID3D11ShaderResourceView* const s_nullSRV[8] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->PSSetShaderResources(0, 8, s_nullSRV);

			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);
			utils::D3D::Instance()->GetDeviceContext()->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, nullptr, _depthStencilView);
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, &s_viewPort);

			s_material->Bind();
			

			s_material->SetMatrix("lightMatrixVP", m_matrixVP.Transpose());
			s_material->ApplyProperty("lightMatrixVP");
			
		}

		void ShadowMap::SetShadowMapDepthStencilView(ID3D11DepthStencilView * dsv)
		{
			_depthStencilView = dsv;
		}

		ID3D11DepthStencilView * ShadowMap::GetShadowMapDepthStencilView()
		{
			return _depthStencilView;
		}

		math::Matrix ShadowMap::GetVPMat()
		{
			return m_matrixVP;
		}

		
	}
}