#include "MemoryAllocation.h"


namespace thomas {

	namespace resource {



		MemoryAllocation::MemoryAllocation()
			: 
			m_allocation(new uint8_t[4096*4]),
			m_constant_memory(4096, m_allocation),
			m_stack_allocation{
		utility::allocator::StackAllocator(4096, m_allocation + 4096),
		utility::allocator::StackAllocator(4096, m_allocation + 4096 * 2),
		utility::allocator::StackAllocator(4096, m_allocation + 4096 * 3)
		}
		{	}

		MemoryAllocation::~MemoryAllocation()
		{
		}

		utility::allocator::StackAllocator & MemoryAllocation::stack(uint32_t index)
		{
			assert(index < NUM_THREAD);
			return m_stack_allocation[index];
		}

	}
}