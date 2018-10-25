#pragma once


/* Clr compatible locks implemented using atomics
*/
#include <atomic>

namespace util
{
	namespace atomics
	{
		class SpinLock {
			/* Simple spinlock, attributed to: 
			 * https://en.cppreference.com/w/cpp/atomic/atomic_flag
			*/
			std::atomic_flag locked = ATOMIC_FLAG_INIT;
		public:
			void lock();
			void unlock();
		};
	}
}