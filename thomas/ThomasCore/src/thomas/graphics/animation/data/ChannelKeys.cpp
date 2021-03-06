#include "ChannelKeys.h"

namespace thomas {
	namespace graphics {
		namespace animation {

#pragma region Channel Key

			ChannelKey::ChannelKey()
				: _time(0.f), _value(nullptr)
			{
			}
			ChannelKey::ChannelKey(float time, float* value)
				: _time(time), _value(value)
			{
			}
			ChannelKey::~ChannelKey() {}
			math::Vector3 ChannelKey::asVec3() const
			{
				return math::Vector3(_value);
			}
			math::Quaternion ChannelKey::asQuat() const
			{
				return math::Quaternion(_value);
			}
			void ChannelKey::set(math::Vector3 value) {
				_value[0] = value.x;
				_value[1] = value.y;
				_value[2] = value.z;
			}
			void ChannelKey::set(math::Quaternion value) {
				_value[0] = value.x;
				_value[1] = value.y;
				_value[2] = value.z;
				_value[3] = value.w;
			}
			/* Set the values of the key.
			*/
			void ChannelKey::set(float time, math::Vector3 value) {
				_time = time;
				_value[0] = value.x;
				_value[1] = value.y;
				_value[2] = value.z;
			}
			/* Set the values of the key.
			*/
			void ChannelKey::set(float time, math::Quaternion value) {
				_time = time;
				_value[0] = value.x;
				_value[1] = value.y;
				_value[2] = value.z;
				_value[3] = value.w;
			}

#pragma endregion

#pragma region Memory Key

			ChannelKeyMemory::ChannelKeyMemory()
				: ChannelKey()
			{
			}
			ChannelKeyMemory::ChannelKeyMemory(unsigned int size)
				: ChannelKey(0.f, new float[size]), _size(size)
			{

			}
			ChannelKeyMemory::ChannelKeyMemory(float time, math::Vector3 data)
				: ChannelKey(time, new float[3]), _size(3)
			{
				ChannelKey::set(time, data);
			}
			ChannelKeyMemory::ChannelKeyMemory(const ChannelKey &key, unsigned int size)
				: ChannelKey(0.f, new float[size]), _size(size)
			{
				//Set the values of the key
				set(key, _size);
			}
			ChannelKeyMemory::ChannelKeyMemory(const ChannelKeyMemory &key)
				: ChannelKey(0.f, new float[key._size]), _size(key._size)
			{
				set(key, _size);
			}

			ChannelKeyMemory::~ChannelKeyMemory()
			{
				delete[] _value;
			}
			ChannelKeyMemory& ChannelKeyMemory::operator = (const ChannelKey &key) {
				set(key, _size);
				return *this;
			}
			ChannelKeyMemory& ChannelKeyMemory::operator= (const ChannelKeyMemory &key) {
				if (this == &key)
					return *this;
				float* tmp = _value;
				_value = new float[key._size];
				_size = key._size;
				set(key, _size);
				delete[] tmp;
				return *this;
			}
			/* Overwrite a number of values from the specific key.
			*/
			void ChannelKeyMemory::set(const ChannelKey& key, unsigned int copyCount) {
				_time = key._time;
				//Copy value
				for (unsigned int i = 0; i < copyCount; i++)
					_value[i] = key._value[i];
				//memcpy(_value, key._value, sizeof(float) * (copyCount = copyCount < _size ? copyCount : _size));
			}

#pragma endregion

		}
	}
}
