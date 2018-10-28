#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>

namespace thomas
{
	namespace utils { namespace buffers { class VertexBuffer; class IndexBuffer; } }
	namespace resource { class TextureCube; class Texture2D; class Shader; }
	namespace graphics
	{
		class SkyBox
		{
		private:
			void GenerateSphere(unsigned horizontalLines, unsigned verticalLines, float radius);
		public:
			SkyBox();
			~SkyBox();
			bool SetSkyMap(resource::Texture2D* tex);
			void Draw();
		private:
			unsigned m_nrOfSphereVertices;
			unsigned m_nrOfSphereFaces;

			std::vector<math::Vector3> m_sphereVerts;
			std::vector<unsigned> m_sphereIndices;
			std::unique_ptr<utils::buffers::VertexBuffer> m_vertBuffer;
			std::unique_ptr<utils::buffers::IndexBuffer> m_indexBuffer;

			resource::TextureCube* m_cubeMap;
			resource::Shader* m_shader;
			
			
		};
	}
}