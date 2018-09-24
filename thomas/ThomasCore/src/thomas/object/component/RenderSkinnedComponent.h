#pragma once
#include "RenderComponent.h"
#include "../../resource/ShaderProperty/ShaderPropertyMatrix.h"
#include "../../utils/Math.h"
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
				/* Access the animated blend tree.
				*/
				graphics::animation::IBlendTree* GetBlendTree();
			public:
			private:
				resource::shaderproperty::ShaderPropertyMatrixArray m_skinArray;
				std::unique_ptr<graphics::animation::AnimatedSkeleton> m_skeleton;
			};
		}
	}
}