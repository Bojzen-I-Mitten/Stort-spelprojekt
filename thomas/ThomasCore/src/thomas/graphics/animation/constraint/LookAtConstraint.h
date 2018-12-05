#pragma once

#include "BoneConstraint.h"
#include "../../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class LookAtConstraint
				: public BoneConstraint {
			public:
				enum AxisConstraint {
					AxisX,
					AxisY,
					AxisZ,
					AxisXYZ
				};


				LookAtConstraint(AxisConstraint);
				~LookAtConstraint();


				

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;
				virtual bool apply(Skeleton& skel, uint32_t boneInd) override;
			public:
				float m_weight;
				math::Vector3 m_target;
				AxisConstraint m_axis;		// Constrained axis, specifies the axis for which rotation are applied.
				math::Axis m_faceAxis;			// Axis faced toward the target.
			private:

			};


		}
	}
}