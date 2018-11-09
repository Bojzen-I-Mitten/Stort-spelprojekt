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
			m_skyMap = nullptr;
			m_shader = graphics::Renderer::Instance()->getShaderList().CreateShader("../Data/FXIncludes/SkyBoxShader.fx");
		}

		SkyBox::~SkyBox()
		{
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
		SkyBox::operator bool() const
		{
			return m_skyMap; //Check null
		}

		void SkyBox::GenerateSphere(unsigned horizontalLines, unsigned verticalLines, float radius)
		{
			std::vector<math::Vector3> sphereVerts;
			std::vector<unsigned> sphereIndices;
			float phi = 0.0f;
			float theta = 0.0f;

			float twoPiDivVerticalLines = math::PI * 2  / verticalLines;
			float piDivHorizontalLines = math::PI / (horizontalLines + 1);

			sphereVerts.push_back(math::Vector3(0.0f, radius, 0.0f));//endpoint 1

			for (unsigned t = 1; t < horizontalLines + 1; ++t)
			{
				theta = t * piDivHorizontalLines;

				for (unsigned p = 0; p < verticalLines; ++p)
				{
					phi = p * twoPiDivVerticalLines;

					sphereVerts.push_back(math::SphericalCoordinate(phi, theta, radius));
				}
			}
			sphereVerts.push_back(math::Vector3(0.0f, -radius, 0.0f)); //endpoint 2
			
			//Create top of sphere
			for (unsigned p = 2; p < verticalLines + 1; ++p)//connect endpoint 1 indices 
			{
				sphereIndices.push_back(0);
				sphereIndices.push_back(p);
				sphereIndices.push_back(p - 1);
			}
			//add last triangle for top (first vert is douplicate)
			sphereIndices.push_back(0);
			sphereIndices.push_back(1);
			sphereIndices.push_back(verticalLines);

			//Create middle segments
			for (unsigned t = 0; t < horizontalLines - 1; ++t)
			{
				for (unsigned p = 2; p < verticalLines + 1; ++p)
				{
					sphereIndices.push_back(t * verticalLines + p);//+ 1 is for to exclude top vert
					sphereIndices.push_back((t + 1) * verticalLines + p);//take vert in next layer
					sphereIndices.push_back(t * verticalLines + p - 1);
					
					sphereIndices.push_back(t * verticalLines + p - 1);
					sphereIndices.push_back((t + 1) * verticalLines + p);
					sphereIndices.push_back((t + 1) * verticalLines + p - 1);
				}

				//Again append douplicate verts
				sphereIndices.push_back(t * verticalLines + 1);
				sphereIndices.push_back((t + 1) * verticalLines + 1);
				sphereIndices.push_back((t + 1) * verticalLines);

				sphereIndices.push_back((t + 1) * verticalLines);
				sphereIndices.push_back((t + 1) * verticalLines + 1);
				sphereIndices.push_back((t + 2) * verticalLines);
			}
			
			//Create bottom of sphere
			for (unsigned p = sphereVerts.size() - verticalLines; p < sphereVerts.size(); ++p)
			{
				sphereIndices.push_back(sphereVerts.size() - 1);
				sphereIndices.push_back(p - 1);
				sphereIndices.push_back(p);
				
			}
			sphereIndices.push_back(sphereVerts.size() - 1);
			sphereIndices.push_back(sphereVerts.size() - 2);
			sphereIndices.push_back(sphereVerts.size() - 1 - verticalLines);
			m_vertBuffer = std::unique_ptr<utils::buffers::VertexBuffer>(
				new utils::buffers::VertexBuffer(sphereVerts));
			m_indexBuffer = std::make_unique<utils::buffers::IndexBuffer>(sphereIndices);
#ifdef _DEBUG
			m_vertBuffer->SetName("SKYBOX_VERTEX");
			m_indexBuffer->SetName("SKYBOX_INDEX");
#endif
		}



		void SkyBox::Draw()
		{
			if (!m_skyMap) return;	// No skymap, don't...
			m_shader->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_shader->BindVertexBuffer(m_vertBuffer.get());
			m_shader->BindIndexBuffer(m_indexBuffer.get());

			m_shader->SetPropertyTextureCube("SkyMap", m_skyMap);

			m_shader->Bind();
			m_shader->SetPass(0);

			
			m_shader->DrawIndexed(m_indexBuffer->GetSize(), 0, 0);
		}
		
	}
}