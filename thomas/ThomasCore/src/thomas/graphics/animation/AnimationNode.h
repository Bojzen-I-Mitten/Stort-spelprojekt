#pragma once

#include <vector>
#include "../../utils/Math.h"
#include "data/TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			class Skeleton;

			constexpr uint32_t ANIM_UPDATED_FLAG = 1;
			//constexpr uint32_t ANIM_APPLIED_FLAG = 2;
			class AnimationNode
			{
			public:
			public:

				Skeleton &m_ref;
				uint32_t m_flag;
				std::vector<int> m_boneMapping;


				uint32_t NumChannel() { return (uint32_t)m_boneMapping.size(); }

				AnimationNode(Skeleton &ref);
				virtual ~AnimationNode();

				virtual void update(float) = 0;
				virtual void resetUpdate();

				virtual math::Vector3 calcBonePosition(unsigned int bone) = 0;
				virtual math::Vector3 calcBoneScale(unsigned int bone) = 0;
				virtual math::Quaternion calcBoneRot(unsigned int bone) = 0;
				virtual void calcFrame(unsigned int bone, math::Vector3& trans, math::Vector3 &scale, math::Quaternion &rot) = 0;
				virtual void calcFrame(TransformComponents * result) = 0;
				math::Matrix calcLocalTransform(unsigned int bone);

				void markUpdated();
				uint32_t isUpdated();
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

				// Inherited via AnimationNode
				virtual void calcFrame(TransformComponents * result) override;
			};
		}
	}
}
