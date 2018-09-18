/* Mattias F 2017*/
#include "Channel.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			ObjectChannel::ObjectChannel()
			{
			}
			ObjectChannel::ObjectChannel(std::vector<Channel> &anim_keys)
				: m_channels(std::move(anim_keys))
			{
			}


			ObjectChannel::~ObjectChannel()
			{
			}

			int ObjectChannel::getNext(float elapsedTime, unsigned int chInd) const
			{
				for (unsigned int i = 0; i < m_channels[chInd].m_keys.size(); i++)
				{
					if (elapsedTime < m_channels[chInd].m_keys[i]._time)
						return i;
				}
				return -1;
			}
			bool ObjectChannel::getNext(float elapsedTime, unsigned int chInd, unsigned int &keyInd) const
			{
				for (; keyInd < m_channels[chInd].m_keys.size(); keyInd++)
				{
					if (elapsedTime < m_channels[chInd].m_keys[keyInd]._time)
						return true;
				}
				return false;
			}
			int ObjectChannel::getPrevious(float elapsedTime, unsigned int chInd) const
			{
				for (unsigned int i = 0; i < m_channels[chInd].m_keys.size(); i++)
				{
					if (elapsedTime >= m_channels[chInd].m_keys[i]._time)
						return i > 0 ? i - 1 : 0; //If no key before the time return the first key.
				}
				return -1;
			}
			bool ObjectChannel::getPrevious(float elapsedTime, unsigned int chInd, unsigned int &keyInd) const
			{
				for (; keyInd > 0; keyInd--)
				{
					if (elapsedTime >= m_channels[chInd].m_keys[keyInd - 1]._time)
						return true;
				}
				return false;
			}

			/* Get a specific key from a channel */
			ChannelKey ObjectChannel::getKey(unsigned int index, unsigned int chInd) const {
				return m_channels[chInd].m_keys[index];
			}

			ChannelKey ObjectChannel::getLast(unsigned int chInd) const
			{
				return m_channels[chInd].m_keys.back();
			}

			ChannelKey ObjectChannel::getFirst(unsigned int chInd) const
			{
				return m_channels[chInd].m_keys.front();
			}


			unsigned int ObjectChannel::numKeys(unsigned int chInd) const {
				return chInd < m_channels.size() ? m_channels[chInd].m_keys.size() : 0;
			}
			unsigned int ObjectChannel::numNodeChannels() const {
				return m_channels.size();
			}

		}
	}
}