#include "ThreadMap.h"
#include "../Common.h"
#include "../ThomasCore.h"
namespace thomas
{
	namespace utils {
		ThreadMap::ThreadMap(uint32_t num_thread)
			: m_numThread(0), m_allocThread(num_thread), m_thread_list(new std::thread::id[num_thread])
		{
		}
		ThreadMap::~ThreadMap()
		{
		}
		void ThreadMap::registerThread()
		{
			std::thread::id id = std::this_thread::get_id();
			size_t i = 0;
			for (; i < m_numThread; i++) {
				if (id == m_thread_list[i])
					return; // Exists..!
			}
			// Simple registering func. meant for 'few' threads.
			if (m_numThread == m_allocThread) {
				LOG("Can't register more threads. Max is: 3");
				return;
			}
			m_thread_list[m_numThread++] == id;
		}

		uint32_t ThreadMap::Thread_Index()
		{
			std::thread::id id = std::this_thread::get_id();
			for (size_t i = 0; i < m_numThread; i++) {
				if (id == m_thread_list[i])
					return i;
			}
			assert(false);	// Ops.. Register thread first!
			return 0;
		}
	}
}