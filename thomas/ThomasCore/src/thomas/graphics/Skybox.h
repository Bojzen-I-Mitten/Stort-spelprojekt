#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "../utils/Buffers.h"
#include "../resource/Shader.h"

namespace thomas
{
	namespace graphics
	{
		class SkyBox
		{
		private:

			void GenerateSphere(unsigned horizontalLines, unsigned verticalLines, float radius);
		public:
			SkyBox();
			~SkyBox();

			void Draw();
		private:
			unsigned m_nrOfSphereVertices;
			unsigned m_nrOfSphereFaces;

			std::vector<math::Vector3> m_sphereVerts;
			std::vector<unsigned> m_sphereIndices;
			std::unique_ptr<utils::buffers::VertexBuffer> m_vertBuffer;
			std::unique_ptr<utils::buffers::IndexBuffer> m_indexBuffer;

			resource::Shader* m_shader;
			
			
		};
	}
}