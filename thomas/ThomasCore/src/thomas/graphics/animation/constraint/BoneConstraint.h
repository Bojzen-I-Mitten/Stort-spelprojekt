#pragma once

#include "../../../utils/Math.h"
#include "../data/TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class Skeleton;

			class BoneConstraint {
			public:
				BoneConstraint() {}
				virtual ~BoneConstraint() {};


				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp,  uint32_t boneInd) = 0;

			};
		}
	}
}