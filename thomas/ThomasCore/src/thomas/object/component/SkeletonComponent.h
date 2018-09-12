#pragma once
#include "Component.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace graphics { namespace animation { class AnimatedSkeleton; } }

	namespace object
	{
		namespace component
		{
			class THOMAS_API SkeletonComponent : public Component
			{
			private:
			public:
				SkeletonComponent();
				virtual ~SkeletonComponent();

				virtual void Update();
			public:
				void SetModel(graphics::animation::AnimatedSkeleton* model);
			public:
				graphics::animation::AnimatedSkeleton* m_skeleton;
			private:
			};
		}
	}
}