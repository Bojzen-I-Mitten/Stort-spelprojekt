#pragma once

#include "../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class AnimationNode
			{
			public:
				AnimationNode();
				virtual ~AnimationNode();

				virtual void update(float) = 0;

				virtual math::Vector3 calcBonePosition(unsigned int bone) = 0;
				virtual math::Vector3 calcBoneScale(unsigned int bone) = 0;
				virtual math::Quaternion calcBoneRot(unsigned int bone) = 0;
				math::Matrix calcLocalTransform(unsigned int bone);

			};
		}
	}
}
