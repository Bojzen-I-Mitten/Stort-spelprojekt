#include "ChadowMapping.h"
#include "../object/component/Camera.h"
#include "../object/component/Transform.h"
#include "../resource/Shader.h"
#include "../resource/Material.h"
#include "../resource/texture/Texture2D.h"
#include "Mesh.h"

namespace thomas
{
	namespace graphics
	{
		std::unique_ptr<resource::Material> ShadowMap::s_material;
		std::unique_ptr<resource::Shader> ShadowMap::s_shader;

		void ShadowMap::InitStatics()
		{
			s_shader = resource::Shader::CreateShader("../Data/FXIncludes/ShadowMapShader.fx");

			s_material = std::make_unique<resource::Material>(s_shader.get());
		}

		ShadowMap::ShadowMap()
		{
			m_depthTexture = std::make_unique<resource::Texture2D>(512, 512, false);

			m_matrixProj = math::Matrix::CreateOrthographicOffCenter(-10, 10, -10, 10, -10, 20);//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

			
			
			/*D3D11_TEXTURE2D_DESC desc = {};
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			desc.Height = 1024;
			desc.Width = 1024;
			desc.MipLevels = 1;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;*/

			D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			
			HRESULT hr = utils::D3D::Instance()->GetDevice()->CreateDepthStencilView(m_depthTexture.get()->GetResource(), &desc, &m_depthStencilView);
			int stopper = 0;
			//utils::D3D::CreateDepthStencilView(); //m_depthTexture.get(),)
		}

		ShadowMap::~ShadowMap()
		{

		}

		void ShadowMap::Update(object::component::Transform& lightTransform, object::component::Camera* camera)
		{
			//https://www.gamedev.net/forums/topic/505893-orthographic-projection-for-shadow-mapping/

			m_matrixView = lightTransform.GetWorldMatrix();


			math::Vector3 corners[8];
			camera->GetFrustrum().GetCorners(corners);

			math::Vector3 frustumCenter = math::Vector3(0, 0, 0);

			for (unsigned i = 0; i < 8; ++i)
			{
				frustumCenter += corners[i];
				math::Vector3::Transform(corners[i], m_matrixView);//transform corner into lightspace
			}
			frustumCenter /= 8;

			//find extreme points
			math::Vector3 maxes;
			math::Vector3 mins;

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

			m_matrixProj = math::Matrix::CreateOrthographicOffCenter(mins.x, maxes.x, mins.y, maxes.y, -maxes.z/* - nearClipOffset*/, -mins.z);

			m_matrixVP = m_matrixProj * m_matrixView;

			s_material->SetMatrix("lightMatrixVP", m_matrixVP);
			
		}

		void ShadowMap::Draw(Mesh * mesh)
		{

			s_material->Draw(mesh);
		}

	}
}