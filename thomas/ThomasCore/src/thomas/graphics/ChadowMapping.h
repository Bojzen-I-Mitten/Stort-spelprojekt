#pragma once
#include <memory>
#include "../utils/Math.h"
#include "../utils/d3d.h"

namespace thomas
{
	namespace resource { class TextureCube; class Texture2D; class Shader; class Material; }
	namespace object { namespace component { class Camera; class Transform; } }
	namespace graphics
	{
		class Mesh;

		class ShadowMap
		{
		public:
			static void InitStatics();
			ShadowMap();
			~ShadowMap();

			void Update(object::component::Transform* lightTransform, object::component::Camera* camera);
			void Draw(Mesh* mesh);
			void Bind();

		private:
			std::unique_ptr<resource::Texture2D> m_depthTexture;
			ID3D11DepthStencilView* m_depthStencilView;


			math::Matrix m_matrixProj;
			math::Matrix m_matrixView;

			math::Matrix m_matrixVP;



			static std::unique_ptr<resource::Material> s_material;
			static std::unique_ptr<resource::Shader> s_shader;
		};
	}
}
