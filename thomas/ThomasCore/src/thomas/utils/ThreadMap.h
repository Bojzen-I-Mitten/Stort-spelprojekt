#pragma once

#include <thread>
#include "../Constants.h"

namespace thomas
{
	namespace utils {

		class ThreadMap {
		public:
			ThreadMap(uint32_t num_thread = MAX_NUM_THREAD);
			~ThreadMap();
			/* Register executing thread (should be called by all threads using core functions).
			*/
			void registerThread();
			/* Get index of executing thread. Should preferably 'not' be used in non-debug code.
			*/
			uint32_t Thread_Index();

		private:
			uint32_t m_numThread, m_allocThread;
			std::thread::id * m_thread_list;


		};
	}
}