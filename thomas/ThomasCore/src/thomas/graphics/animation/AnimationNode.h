#pragma once

#include <vector>
#include "../../utils/Math.h"
#include "data/TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			class Skeleton;

			class AnimationNode
			{
			public:

				Skeleton &m_ref;
				uint32_t m_numChannel;
				std::vector<int> m_boneMapping;

				AnimationNode(Skeleton &ref);
				virtual ~AnimationNode();

				virtual void update(float) = 0;

				virtual math::Vector3 calcBonePosition(unsigned int bone) = 0;
				virtual math::Vector3 calcBoneScale(unsigned int bone) = 0;
				virtual math::Quaternion calcBoneRot(unsigned int bone) = 0;
				virtual void calcFrame(unsigned int bone, math::Vector3& trans, math::Vector3 &scale, math::Quaternion &rot) = 0;
				virtual void calcFrame(TransformComponents * result) = 0;
				math::Matrix calcLocalTransform(unsigned int bone);


				//int map(int bone) { return m_boneMapping[bone]; }
			};

			class BindPoseNode : public AnimationNode {
			public:
				BindPoseNode(Skeleton &ref);
				virtual ~BindPoseNode();

				// Inherited via AnimationNode
				virtual void update(float) override;
				virtual math::Vector3 calcBonePosition(unsigned int bone) override;
				virtual math::Vector3 calcBoneScale(unsigned int bone) override;
				virtual math::Quaternion calcBoneRot(unsigned int bone) override;

				virtual void calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot) override;
			};
		}
	}
}
