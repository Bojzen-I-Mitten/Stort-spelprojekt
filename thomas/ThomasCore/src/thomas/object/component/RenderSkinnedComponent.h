#pragma once
#include "RenderComponent.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace graphics { namespace animation { class AnimatedSkeleton; } }

	namespace object
	{
		namespace component
		{
			class THOMAS_API RenderSkinnedComponent : public RenderComponent
			{
			private:
			public:
				RenderSkinnedComponent();
				virtual ~RenderSkinnedComponent();

				virtual void Update();
			public:
				virtual void SetModel(resource::Model* model);

			public:
				std::unique_ptr<graphics::animation::AnimatedSkeleton> m_skeleton;
			private:
			};
		}
	}
}