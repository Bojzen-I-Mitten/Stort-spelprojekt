#pragma once
/* Mattias F 2017
	- Modified 2018
*/
#include "../../../utils/Math.h"

namespace thomas {
	namespace animation {

		/* Key in a animated channel holding reference to a set of floats and the relative time of influence.
		*/
		class ChannelKey {
		public:

			/* Animation time the key value influences it's channel */
			float _time;
			/* Referenced key value (if not an memory key) */
			float* _value;

			ChannelKey();
			ChannelKey(float time, float* value);
			~ChannelKey();
			math::Vector3 asVec3() const;
			math::Quaternion asQuat() const;

			/* Set the value of the key */
			void set(math::Vector3 value);
			/* Set the value of the key */
			void set(math::Quaternion value);
			/* Set the values of the key. */
			void set(float time, math::Vector3 value);
			/* Set the values of the key. */
			void set(float time, math::Quaternion value);
		};
		/* Channel key owning it's memory pointer. Note that keys need to match when use and size is determined by the larger structure.
		*/
		class ChannelMemoryKey
			: public ChannelKey {
		public:

			/* Number of values in the key
			*/
			unsigned int _size;

			ChannelMemoryKey();
			ChannelMemoryKey(unsigned int size);
			ChannelMemoryKey(float time, math::Vector3 data);
			ChannelMemoryKey(const ChannelKey &key, unsigned int size);
			ChannelMemoryKey(const ChannelMemoryKey &key);
			~ChannelMemoryKey();

			ChannelMemoryKey& operator= (const ChannelKey &key);
			ChannelMemoryKey& operator= (const ChannelMemoryKey &key);

			/* Overwrite a number of values from the specific key.
			*/
			void set(const ChannelKey& key, unsigned int copyCount);
			using ChannelKey::set; //Make parent set functions visible
		};
	}
}