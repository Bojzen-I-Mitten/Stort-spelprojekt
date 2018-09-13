#pragma once
#include <vector>
#include <memory>
#include "Bone.h"
#include "AnimationData.h"
#include "ChannelKeys.h"


namespace thomas {
	namespace graphics {
		namespace animation {

			struct AnimSize {
				size_t _numFloats;
				size_t _numChannels;
				size_t _numNodeChannels;
				size_t _numBones;
			};
			struct AnimationConstruct {
				size_t _dataInd;
				float* m_data;
				std::vector<std::vector<Channel>> _keys;
			public:

				AnimationConstruct(AnimSize size);
				~AnimationConstruct();

				std::shared_ptr<AnimationData> generateAnim(const char* name, float duration);

				void insertQuat(int ch, int nodeCh, float time, const float quat[4]);
				void insertVector(int ch, int nodeCh, float time, const float vector[3]);
			};
		}
	}
}