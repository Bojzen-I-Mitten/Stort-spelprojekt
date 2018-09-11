#pragma once
#include <vector>
#include <string>
#include "KeyFrame.h"
#include "Channel.h"

namespace thomas {
	namespace animation {

		class Animation
		{
		private:
			/* The animated channels, one for each node. */
			std::vector<BoneChannel> _channels;
			/* Array holding the key data */
			std::unique_ptr<float> _data;
		public:
			/* Name of animation */
			const std::string _name;
			/* The duration of the animation */
			const float _duration;

			Animation(const std::string &name, float duration, std::vector<BoneChannel> &channels, std::unique_ptr<float>& data);
			~Animation();
			/* Get the channel related to a bone */
			const BoneChannel& operator[](unsigned int bone);
		};
	}
}

