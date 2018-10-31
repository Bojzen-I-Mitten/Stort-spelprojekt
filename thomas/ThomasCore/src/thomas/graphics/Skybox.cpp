#include "Skybox.h"
#include "Renderer.h"
#include "../utils/d3d.h"
#include "../utils/Buffers.h"
#include "../resource/texture/Texture2D.h"
#include "../resource/texture/TextureCube.h"
#include "../resource/Shader.h"

namespace thomas
{
	namespace graphics
	{
		SkyBox::SkyBox()
		{

			GenerateSphere(10, 10, 5.0f);
			m_vertBuffer = std::make_unique<utils::buffers::VertexBuffer>(m_sphereVerts);
			m_indexBuffer = std::make_unique<utils::buffers::IndexBuffer>(m_sphereIndices);
			
			m_skyMap = new resource::TextureCube("../Data/yokohama_skybox.dds");
			//m_skyMap = new resource::TextureCube("../Data/fullmoon_skybox.dds");
			m_shader = graphics::Renderer::Instance()->getShaderList().CreateShader("../Data/FXIncludes/SkyBoxShader.fx");
		}

		SkyBox::~SkyBox()
		{
			/*SAFE_DELETE(m_cubeMap);
			SAFE_DELETE(m_skyMap);

			SAFE_RELEASE(m_vertBuffer);
			SAFE_RELEASE(m_indexBuffer);

			m_sphereIndices.clear();
			m_sphereVerts.clear();*/
		}

		bool SkyBox::SetSkyMap(resource::TextureCube * tex)
		{

			m_skyMap = tex;
			
			return true;
		}

		resource::TextureCube * SkyBox::GetSkyMap()
		{
			return m_skyMap;
		}

		void SkyBox::GenerateSphere(unsigned horizontalLines, unsigned verticalLines, float radius)
		{
			float phi = 0.0f;
			float theta = 0.0f;

			float twoPiDivVerticalLines = math::PI * 2  / verticalLines;
			float piDivHorizontalLines = math::PI / (horizontalLines + 1);

			m_sphereVerts.push_back(math::Vector3(0.0f, radius, 0.0f));//endpoint 1

			for (unsigned t = 1; t < horizontalLines + 1; ++t)
			{
				theta = t * piDivHorizontalLines;

				for (unsigned p = 0; p < verticalLines; ++p)
				{
					phi = p * twoPiDivVerticalLines;

					m_sphereVerts.push_back(math::SphericalCoordinate(phi, theta, radius));
				}
			}
			m_sphereVerts.push_back(math::Vector3(0.0f, -radius, 0.0f)); //endpoint 2
			
			//Create top of sphere
			for (unsigned p = 2; p < verticalLines + 1; ++p)//connect endpoint 1 indices 
			{
				m_sphereIndices.push_back(0);
				m_sphereIndices.push_back(p);
				m_sphereIndices.push_back(p - 1);
			}
			//add last triangle for top (first vert is douplicate)
			m_sphereIndices.push_back(0);
			m_sphereIndices.push_back(1);
			m_sphereIndices.push_back(verticalLines);

			//Create middle segments
			for (unsigned t = 0; t < horizontalLines - 1; ++t)
			{
				for (unsigned p = 2; p < verticalLines + 1; ++p)
				{
					m_sphereIndices.push_back(t * verticalLines + p);//+ 1 is for to exclude top vert
					m_sphereIndices.push_back((t + 1) * verticalLines + p);//take vert in next layer
					m_sphereIndices.push_back(t * verticalLines + p - 1);
					
					m_sphereIndices.push_back(t * verticalLines + p - 1);
					m_sphereIndices.push_back((t + 1) * verticalLines + p);
					m_sphereIndices.push_back((t + 1) * verticalLines + p - 1);
				}

				//Again append douplicate verts
				m_sphereIndices.push_back(t * verticalLines + 1);
				m_sphereIndices.push_back((t + 1) * verticalLines + 1);
				m_sphereIndices.push_back((t + 1) * verticalLines);

				m_sphereIndices.push_back((t + 1) * verticalLines);
				m_sphereIndices.push_back((t + 1) * verticalLines + 1);
				m_sphereIndices.push_back((t + 2) * verticalLines);
			}
			
			//Create bottom of sphere
			for (unsigned p = m_sphereVerts.size() - verticalLines; p < m_sphereVerts.size(); ++p)
			{
				m_sphereIndices.push_back(m_sphereVerts.size() - 1);
				m_sphereIndices.push_back(p - 1);
				m_sphereIndices.push_back(p);
				
			}
			m_sphereIndices.push_back(m_sphereVerts.size() - 1);
			m_sphereIndices.push_back(m_sphereVerts.size() - 2);
			m_sphereIndices.push_back(m_sphereVerts.size() - 1 - verticalLines);
		}



		void SkyBox::Draw()
		{

			m_shader->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_shader->BindVertexBuffer(m_vertBuffer.get());
			m_shader->BindIndexBuffer(m_indexBuffer.get());

			m_shader->SetPropertyTextureCube("SkyMap", m_skyMap);

			m_shader->Bind();
			m_shader->SetPass(0);

			
			m_shader->DrawIndexed(m_sphereIndices.size(), 0, 0);
		}
		
	}
}