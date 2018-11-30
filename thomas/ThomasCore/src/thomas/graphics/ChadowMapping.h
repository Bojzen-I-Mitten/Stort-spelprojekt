#pragma once
#include <memory>
#include "../utils/Math.h"
#include "../utils/d3d.h"

namespace thomas
{
	namespace resource { class TextureCube; class Texture2DArray; class Shader; class Material; }
	namespace object { namespace component { class Camera; class Transform; } }
	namespace graphics
	{
		namespace render { struct RenderCommand; }
		class Mesh;


		class ShadowMap
		{
		public:
			static void InitStatics(unsigned size);
			static void Destroy();
			ShadowMap();
			~ShadowMap();

			void UpdateShadowBox(object::component::Transform* lightTransform, object::component::Camera* camera);
			
			void Draw(render::RenderCommand& renderCommand);
			void Bind();
			void SetShadowMapDepthStencilView(ID3D11DepthStencilView* dsv);
			ID3D11DepthStencilView* GetShadowMapDepthStencilView();

			math::Matrix GetVPMat();
		private:
			ID3D11DepthStencilView* _depthStencilView;
			
			math::Matrix m_matrixProj;
			math::Matrix m_matrixView;
			math::Matrix m_matrixVP;

			unsigned m_nrOfCascades;

			static std::unique_ptr<resource::Material> s_material;
			static std::unique_ptr<resource::Material> s_animmaterial;
			static std::unique_ptr<resource::Shader> s_shader;
			static std::unique_ptr<resource::Shader> s_animshader;
			
			static D3D11_VIEWPORT s_viewPort;
		};
	}
}
