#pragma once
#include "BoneConstraint.h"

namespace thomas {
	namespace object {
		namespace component {
			class Transform;
		}
	}
	namespace graphics {
		namespace animation {

			/* Constraint for bone m_position. Currently limits Y value only
			*/
			class PositionConstraint
				: public BoneConstraint
			{
			public:
				PositionConstraint();
				~PositionConstraint();

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;
				virtual bool apply(Skeleton& skel, uint32_t boneInd) override;
				/* Position used for constraining the target
				*/
				math::Vector3 m_position;
				float m_distance;
				bool m_active;
			private:
			};

		}
	}
}