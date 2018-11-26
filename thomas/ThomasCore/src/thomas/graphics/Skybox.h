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
			bool SetSkyMap(resource::TextureCube* tex);
			resource::TextureCube* GetSkyMap();
			void Draw();
		private:

			std::unique_ptr<utils::buffers::VertexBuffer> m_vertBuffer;
			std::unique_ptr<utils::buffers::IndexBuffer> m_indexBuffer;

			resource::TextureCube* m_skyMap;
			resource::Shader* m_shader;
			
			operator bool() const;

			
		};
	}
}