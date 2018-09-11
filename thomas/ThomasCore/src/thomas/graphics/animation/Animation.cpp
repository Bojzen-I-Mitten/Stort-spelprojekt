#include "Animation.h"


namespace thomas {
	namespace animation {

		Animation::Animation(const std::string &name, float duration, std::vector<BoneChannel> &channels, std::unique_ptr<float>& data)
			: _channels(std::move(channels)), _data(std::move(data)), _name(name), _duration(duration)
		{
		}


		Animation::~Animation()
		{
		}


		const BoneChannel& Animation::operator[](unsigned int bone) {
			return _channels[bone];
		}
	}
}