#include "AnimationConstruct.h"

namespace thomas {
	namespace animation {

		AnimationConstruct::AnimationConstruct(AnimSize size)
			: _dataInd(0), _data(new float[size._numFloats]), _keys(size._numBones)
		{
			for (unsigned int i = 0; i < size._numBones; i++)
			{
				_keys[i] = std::vector<std::vector<ChannelKey>>(size._numNodeChannels);
			}
		}

		AnimationConstruct::~AnimationConstruct()
		{
			delete[] _data;
		}

		std::shared_ptr<Animation> AnimationConstruct::generateAnim(const char* name, float duration) {
			std::unique_ptr<float> ptr(_data);
			_data = nullptr;
			std::vector<BoneChannel> channels(_keys.size());
			for (unsigned int i = 0; i < _keys.size(); i++)
				channels[i] = BoneChannel(_keys[i]);
			return std::shared_ptr<Animation>(new Animation(name, duration, channels, ptr));
		}

		void AnimationConstruct::insertQuat(int ch, int nodeCh, float time, const float value[4]) {
			_keys[ch][nodeCh].push_back(ChannelKey(time, _data + _dataInd));
			_data[_dataInd] = value[0];
			_data[_dataInd + 1] = value[1];
			_data[_dataInd + 2] = value[2];
			_data[_dataInd + 3] = value[3];
			_dataInd += 4;
		}
		void AnimationConstruct::insertVector(int ch, int nodeCh, float time, const float value[3]) {
			_keys[ch][nodeCh].push_back(ChannelKey(time, _data + _dataInd));
			_data[_dataInd] = value[0];
			_data[_dataInd + 1] = value[1];
			_data[_dataInd + 2] = value[2];
			_dataInd += 3;
		}
	}
}