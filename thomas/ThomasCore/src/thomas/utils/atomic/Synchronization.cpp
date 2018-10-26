#include "Synchronization.h"


namespace util
{
	namespace atomics
	{
		void SpinLock::lock() {
			while (locked.test_and_set(std::memory_order_acquire)) { ; }
		}
		void SpinLock::unlock() {
			locked.clear(std::memory_order_release);
		}
	}
}