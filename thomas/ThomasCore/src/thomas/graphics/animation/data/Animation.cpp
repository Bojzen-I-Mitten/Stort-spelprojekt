#include "Animation.h"


namespace thomas {
	namespace graphics {
		namespace animation {

			Animation::Animation(const std::string &name, float duration, std::vector<ObjectChannel> &channels, std::unique_ptr<float>& data)
				: m_channels(std::move(channels)), m_data(std::move(data)), m_name(name), m_duration(duration)
			{
			}


			Animation::~Animation()
			{
			}


			const ObjectChannel& Animation::operator[](unsigned int bone) {
				return m_channels[bone];
			}
		}
	}
}