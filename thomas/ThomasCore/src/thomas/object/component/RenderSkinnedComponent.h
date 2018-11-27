#pragma once
#include "RenderComponent.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace utils
	{
		namespace buffers {
			class StructuredBuffer;
		}
	}
	namespace graphics { 
		namespace animation { 
		class AnimatedSkeleton;
		class IBlendTree;
		}
	}
	namespace object
	{
		class Camera;
		namespace component
		{
			class RenderSkinnedComponent : public RenderComponent
			{
			private:
			public:
				RenderSkinnedComponent();
				virtual ~RenderSkinnedComponent();
				virtual void Update();
			protected:
				void SubmitPart(Camera* camera, unsigned int i, const thomas::resource::shaderproperty::ShaderPropertyStatic* property_data, uint32_t num_prop);
			public:
				virtual bool SetModel(resource::Model* model);
				virtual void SetMaterial(int meshIndex, resource::Material* material);

				/* Access the animated blend tree.
				*/
				graphics::animation::IBlendTree* GetBlendTree();
			public:
			private:
				resource::shaderproperty::ShaderPropertyStatic* m_skinInfo;
				std::unique_ptr<graphics::animation::AnimatedSkeleton> m_skeleton;
				utils::buffers::StructuredBuffer* sb;

				void applySkin();
			};
		}
	}
}