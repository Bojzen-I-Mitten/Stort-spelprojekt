#pragma once
#include <memory>
#include <vector>
#include "ChannelKeys.h"

namespace thomas {
	namespace animation {
		/* An animated channel, contains the animated keys for each value channel animated for the node.
		*/
		class BoneChannel
		{
		private:
			/* Set of different keys included in the channel.
			 * Each set holds the set of animated keys for a value channel over the duration of the animation.
			 */
			std::vector<std::vector<ChannelKey>> _channelKeys;
		public:
			BoneChannel();
			BoneChannel(std::vector<std::vector<ChannelKey>> &channelKeys);
			~BoneChannel();
			/* Get next keyframe for the specific animation time. If there is no more -1 is returned
			*/
			int getNext(float elapsedTime, unsigned int chInd) const;
			/* Get previous keyframe for the specific animation time. If there is none -1 is returned
			*/
			int getPrevious(float elapsedTime, unsigned int chInd) const;
			/* Get a specific key from a channel */
			ChannelKey getKey(unsigned int index, unsigned int chInd) const;
			/* Get the number of keys in a specific node channel */
			unsigned int numKeys(unsigned int chType) const;
			/* Get the number of animated channels in the channel node */
			unsigned int numNodeChannels() const;
		};
	}
}

