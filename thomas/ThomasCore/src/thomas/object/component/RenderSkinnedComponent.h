#pragma once
#include "RenderComponent.h"
#include "../../utils/Math.h"

#include <memory>
namespace thomas
{
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

				void applySkin();
			};
		}
	}
}