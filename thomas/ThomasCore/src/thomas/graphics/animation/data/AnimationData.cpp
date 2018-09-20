#include "AnimationData.h"
#include "Skeleton.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimationData::AnimationData(const std::string &name, float duration, std::vector<ObjectChannel> &channels, std::unique_ptr<float>& data)
				: m_channels(std::move(channels)), m_data(std::move(data)), m_name(name), m_duration(duration)
			{
			}

			
			AnimationData::~AnimationData()
			{
			}

			size_t AnimationData::numChannel() const
			{
				return m_channels.size();
			}


			const ObjectChannel& AnimationData::operator[](unsigned int bone) const {
				return m_channels[bone];
			}
		}
	}
}