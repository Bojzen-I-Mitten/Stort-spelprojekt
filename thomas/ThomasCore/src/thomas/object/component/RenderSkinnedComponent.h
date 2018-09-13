#pragma once
#include "RenderComponent.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace graphics { namespace animation { class AnimatedSkeleton; } }
	namespace object
	{
		class Camera;
		namespace component
		{
			class THOMAS_API RenderSkinnedComponent : public RenderComponent
			{
			private:
			public:
				RenderSkinnedComponent();
				virtual ~RenderSkinnedComponent();
				virtual void Update();
			protected:
				virtual void SubmitPart(Camera* camera, unsigned int i);
			public:
				virtual void SetModel(resource::Model* model);

			public:
				std::unique_ptr<graphics::animation::AnimatedSkeleton> m_skeleton;
				std::unique_ptr<resource::shaderproperty::ShaderProperty> m_skinArray;
			private:
			};
		}
	}
}