#pragma once
#include "Component.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace graphics {struct RenderPair; }
	namespace resource { class Model; class Material; }
	namespace object
	{
		namespace component
		{
			class Camera;
			class THOMAS_API RenderComponent : public Component
			{
			public:
				RenderComponent();
				virtual void SetModel(resource::Model* model);
				
				resource::Model * GetModel();
				void Update();

				void SetMaterial(int meshIndex, resource::Material* material);
				resource::Material* GetMaterial(int meshIndex);

				void OnDrawGizmos();
				void SubmitToRenderer(Camera* camera);
				void OnDestroy();
				static std::vector<RenderComponent*> GetAllRenderComponents();
			public:
				resource::Model* m_model;
				math::BoundingOrientedBox m_bounds;
			protected:
				virtual void SubmitPart(Camera* camera, unsigned int i);
			protected:
				std::vector<resource::Material*> m_materials;
			private:
				static std::vector<RenderComponent*> s_renderComponents;
				//graphics::Geometry* m_geometry;
			};
		}
	}
}