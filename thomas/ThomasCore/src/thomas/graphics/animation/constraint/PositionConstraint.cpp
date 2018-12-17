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
				math::Vector3 s = math::extractAxisScale(objectPose[boneInd]);
				// If (transform is childTransform)
				objectPose[boneInd]._42 = m_position.y;
			}

			bool PositionConstraint::apply(Skeleton & skel, uint32_t boneInd)
			{
				return true;
			}

		}
	}
}