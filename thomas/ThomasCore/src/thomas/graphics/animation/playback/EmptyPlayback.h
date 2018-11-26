#pragma once
#include <memory>
#include <vector>
#include "../AnimationNode.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class Playback;
			class AnimationData;
			class Skeleton;

			class EmptyPlayback : public AnimationNode
			{
			public:
				EmptyPlayback(Skeleton &skel);
				virtual ~EmptyPlayback();	


				virtual void update(float);

				virtual math::Vector3 calcBonePosition(unsigned int bone);
				virtual math::Vector3 calcBoneScale(unsigned int bone);
				virtual math::Quaternion calcBoneRot(unsigned int bone);
				virtual void calcFrame(unsigned int bone, math::Vector3& trans, math::Vector3 &scale, math::Quaternion &rot);
				virtual void calcFrame(TransformComponents * result);
			};
		}
	}
}