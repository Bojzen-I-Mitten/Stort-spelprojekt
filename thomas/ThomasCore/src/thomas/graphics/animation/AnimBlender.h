#pragma once
#include "AnimationNode.h"
#include <utility>
#include "../../Constants.h"
#include <memory>

namespace thomas {
	namespace graphics {
		namespace animation {
			
			class Playback;
			class AnimationData;
			class Skeleton;
			class WeightMixer;
			/* Flexible blending node
			*/
			class AnimBlender : public AnimationNode 
			{
			public:
			private:

				AnimationNode* m_nodes[MAX_ANIMATION_BLEND_NODE];
				uint32_t m_NumNode;
				std::unique_ptr<WeightMixer> m_weights;


			public:
				AnimBlender(Skeleton &skel);
				AnimBlender(Skeleton &skel, uint32_t numNodes);

				virtual ~AnimBlender();

				void setWeightMixer(WeightMixer* mixer);
				void generateLinearMixer(float durationPerNode);

				void pushAnimation(AnimationNode* node);
				void setAnimation(uint32_t index, AnimationNode* n);
				void constructMapping();

				// Inherited via AnimationNode
				virtual void update(float) override;
				virtual void resetUpdate() override;

				virtual math::Vector3 calcBonePosition(unsigned int bone) override;
				virtual math::Vector3 calcBoneScale(unsigned int bone) override;
				virtual math::Quaternion calcBoneRot(unsigned int bone) override;
				virtual void calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot) override;
				virtual void calcFrame(TransformComponents * result) override;
			};
		}
	}
}