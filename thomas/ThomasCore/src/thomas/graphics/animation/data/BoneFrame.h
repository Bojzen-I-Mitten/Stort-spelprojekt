#pragma once
#include "KeyFrame.h"

namespace thomas {
	namespace animation {
		const unsigned int NUM_BONE_CHANNEL = 3;
		const unsigned int SIZE_BONE_CHANNEL[3] = { 3, 3, 4 };

		class BoneFrame
			: public KeyFrame<3>
		{
		public:
			BoneFrame();
			~BoneFrame();
			/* Set an initial pose for each bone */
			void init(const math::Matrix& pose);
			/* Lerps from the keyframe to the next one. Updating the active keys.
			*/
			math::Matrix lerp(float eT);
			/* Store the current animation state in the specific node channel
			*/
			virtual void storeState(unsigned int type, float eT);
		};
	}
}

