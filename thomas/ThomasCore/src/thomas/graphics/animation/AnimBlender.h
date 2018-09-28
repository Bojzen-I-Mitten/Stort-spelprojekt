#pragma once
#include "AnimationNode.h"
#include <utility>
namespace thomas {
	namespace graphics {
		namespace animation {
			
			class Playback;
			class AnimationData;
			class Skeleton;
			class WeightMixer;

			class AnimBlender : public AnimationNode 
			{
			private:

				AnimationNode* m_nodes[10];
				uint32_t m_NumNode;
				WeightMixer * m_weights;

			public:
				AnimBlender(Skeleton &skel);
				virtual ~AnimBlender();

				void constructMapping();


				void pushAnimation(AnimationNode* node);

				// Inherited via AnimationNode
				virtual void update(float) override;
				virtual math::Vector3 calcBonePosition(unsigned int bone) override;
				virtual math::Vector3 calcBoneScale(unsigned int bone) override;
				virtual math::Quaternion calcBoneRot(unsigned int bone) override;
				virtual void calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot) override;
				virtual void calcFrame(TransformComponents * result) override;
			};
		}
	}
}