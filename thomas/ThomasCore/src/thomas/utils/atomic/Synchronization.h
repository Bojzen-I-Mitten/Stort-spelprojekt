#pragma once


/* Clr compatible locks implemented using atomics
*/
#include <atomic>

namespace thomas
{

	namespace utils
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

			/* Lock for small functions
			*/
			struct Lock
			{
			private:
				SpinLock& m_mtx;
			public:
				Lock(SpinLock& mtx);
				~Lock();
			};
		}
	}
}