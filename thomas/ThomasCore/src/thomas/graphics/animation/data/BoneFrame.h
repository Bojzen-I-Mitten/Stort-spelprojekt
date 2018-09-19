#pragma once
#include "KeyFrame.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			const unsigned int NUM_BONE_CHANNEL = 3;
			const unsigned int SIZE_BONE_CHANNEL[3] = { 3, 3, 4 };

			class BoneFrame
				: public KeyFrame<3>
			{
			public:
				BoneFrame();
				~BoneFrame();
				void init(math::Vector3 pos, math::Vector3 scale, math::Quaternion quat);
				/* Set an initial pose for each bone */
				void init(const math::Matrix& pose);
				math::Vector3 lerpCoordinate(float eT);
				math::Vector3 lerpScale(float eT);
				math::Quaternion lerpRotation(float eT);
				/* Lerps from the keyframe to the next one. Updating the active keys.
				*/
				void lerpFrame(float eT, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & quat);
				math::Matrix lerp(float eT);


				/* Store the current animation state in the specific node channel
				*/
				virtual void storeState(unsigned int type, float eT);

			private:
				unsigned int m_last_frame;
			};
		}
	}
}

