#pragma once
#include "Component.h"
#include "../../utils/Math.h"
#include "../../resource/ShaderProperty/ShaderPropertyFunction.h"
namespace thomas
{
	namespace graphics {struct RenderPair; }
	namespace resource {
		class Model; class Material;
		namespace shaderproperty {
			class ShaderProperty;
		}
	}
	namespace object
	{
		namespace component
		{
			class Camera;
			class THOMAS_API RenderComponent : public Component
			{
			public:
				RenderComponent();
				virtual bool SetModel(resource::Model* model);
				
				resource::Model * GetModel();
				void Update();

				void SetMaterial(resource::Material * material);
				

				virtual void SetMaterial(int meshIndex, resource::Material* material);
				resource::Material* GetMaterial(int meshIndex);

				void OnDrawGizmos();
				void SubmitToRenderer(Camera* camera);
				void OnDestroy();
				static std::vector<RenderComponent*> GetAllRenderComponents();
				static void ClearList();
			public:
				resource::Model* m_model;
				math::BoundingOrientedBox m_bounds;
			protected:
				void SubmitPart(Camera* camera, unsigned int i);
			protected:
				std::vector<resource::Material*> m_materials;
				/*	Insert a shader property for rendering component.
				 * WARNING!: Append only on initialization (or make safe with parameters passed to Renderer) 
				*/
				resource::shaderproperty::ShaderPropertyStatic& insertProperty(resource::shaderproperty::ShaderPropertyStatic prop);
			private:
				static std::vector<RenderComponent*> s_renderComponents;
				std::vector<resource::shaderproperty::ShaderPropertyStatic> m_properties;
				//graphics::Geometry* m_geometry;
			};
		}
	}
}