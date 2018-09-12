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
			std::vector<ObjectChannel> m_channels;
			/* Array holding the key data */
			std::unique_ptr<float> m_data;
		public:
			/* Name of animation */
			const std::string m_name;
			/* The duration of the animation */
			const float m_duration;

			Animation(const std::string &name, float duration, std::vector<ObjectChannel> &channels, std::unique_ptr<float>& data);
			~Animation();
			/* Get the channel related to a bone */
			const ObjectChannel& operator[](unsigned int bone);
		};
	}
}

