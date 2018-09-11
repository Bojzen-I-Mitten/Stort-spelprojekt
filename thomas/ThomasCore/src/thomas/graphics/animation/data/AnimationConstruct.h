#pragma once
#include <vector>
#include <memory>
#include "Bone.h"
#include "Animation.h"
#include "ChannelKeys.h"


namespace thomas {
	namespace animation {

		struct AnimSize {
			size_t _numFloats;
			size_t _numChannels;
			size_t _numNodeChannels;
			size_t _numBones;
		};
		struct AnimationConstruct {
			size_t _dataInd;
			float* _data;
			std::vector<std::vector<std::vector<ChannelKey>>> _keys;
		public:

			AnimationConstruct(AnimSize size);
			~AnimationConstruct();

			std::shared_ptr<Animation> generateAnim(const char* name, float duration);

			void insertQuat(int ch, int nodeCh, float time, const float quat[4]);
			void insertVector(int ch, int nodeCh, float time, const float vector[3]);
		};
	}
}