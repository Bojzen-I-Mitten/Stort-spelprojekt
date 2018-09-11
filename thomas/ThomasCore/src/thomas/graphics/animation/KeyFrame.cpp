#include "KeyFrame.h"


namespace thomas {
	namespace animation {

		/* Lerp the keys as vec3 with the specific amount */
		math::Vector3 lerpVec3(const ChannelKey& from, const ChannelKey& to, float amount) {
			return from.asVec3() * (1.f - amount) + to.asVec3() * amount;
		}
		/* Slerp the keys as quaternions with the specific amount */
		math::Quaternion slerp(const ChannelKey& from, const ChannelKey& to, float amount) {
			return math::Quaternion::Slerp(from.asQuat(), to.asQuat(), amount);
		}
	}
}