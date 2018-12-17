#include "PositionConstraint.h"
#include "../data/Skeleton.h"
#include "../../../object/component/Transform.h"

namespace thomas {
	namespace graphics {
		namespace animation {


			PositionConstraint::PositionConstraint()
			{
			}

			PositionConstraint::~PositionConstraint()
			{
			}

			void PositionConstraint::execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd)
			{
				math::Vector3 v = objectPose[boneInd].Translation() - m_position;
				// If (transform is childTransform)
				if (v.Length() > m_distance)
				{
					// Find the height y at m_distance from m_position:
					float h = m_distance * m_distance - math::square(v.x) - math::square(v.z);
					float y = h < 0.f ? 0.f : std::sqrtf(h);	// If h < 0 pos is unreachable at the distance
					objectPose[boneInd]._42 = m_position.y + y;
				}
			}

			bool PositionConstraint::apply(Skeleton & skel, uint32_t boneInd)
			{
				return true;
			}

		}
	}
}