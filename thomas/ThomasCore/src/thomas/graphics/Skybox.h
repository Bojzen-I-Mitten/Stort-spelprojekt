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
			//bool SetSkyMap(resource::Texture2D* tex);
			//resource::Texture2D* GetSkyMap();
			void Draw();
		private:

			std::vector<math::Vector3> m_sphereVerts;
			std::vector<unsigned> m_sphereIndices;
			std::unique_ptr<utils::buffers::VertexBuffer> m_vertBuffer;
			std::unique_ptr<utils::buffers::IndexBuffer> m_indexBuffer;

			//resource::Texture2D* m_skyMap;
			//resource::Texture2D* m_skyMap;
			resource::TextureCube* m_cubeMap;
			resource::Shader* m_shader;
			
			
		};
	}
}