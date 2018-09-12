#include "SkeletonComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			SkeletonComponent::SkeletonComponent() {

			}
			SkeletonComponent::~SkeletonComponent() {

			}
			void SkeletonComponent::Update()
			{
				if (m_skeleton)
					m_skeleton->update(0.f);
			}
			void SkeletonComponent::SetModel(graphics::animation::AnimatedSkeleton * skele)
			{
				m_skeleton = skele;
			}
		}
	}
}
