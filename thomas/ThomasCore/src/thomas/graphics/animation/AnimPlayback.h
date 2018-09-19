#pragma once
#include <memory>
#include <vector>
#include "AnimationNode.h"
#include "data/BoneFrame.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class Playback;
			class AnimationData;
			class Skeleton;

			class AnimPlayback : public AnimationNode
			{
			public:


				AnimPlayback(Skeleton &skel);	// Pose
				AnimPlayback(Skeleton &skel, std::unique_ptr<Playback>& playback, AnimationData &anim);
				virtual ~AnimPlayback();

				virtual void update(float dT);

			private:

				AnimationData *m_anim;
				std::unique_ptr<Playback> m_playback;
				std::vector<BoneFrame> m_channel;		// Current keys active in the animation for each channel
				std::vector<unsigned int> m_boneMap;


				virtual math::Vector3 calcBonePosition(unsigned int bone);
				virtual math::Vector3 calcBoneScale(unsigned int bone);
				virtual math::Quaternion calcBoneRot(unsigned int bone);

				virtual void calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot);






				/* Recalculates the end time when animation is looped */
				void loopRefit();
				/* Set the animation from the name, returns true if animation found */
				//bool setAnim(const std::string& name, PlayType runType);
				/* Pose the skeleton at the specific point of animation. Blends into the pose over the animation time specified. */
				bool setAnimPose(const std::string& name, float poseAt, float animTime);
			};
		}
	}
}

