#include "MemoryAllocation.h"


namespace thomas {

	namespace resource {



		MemoryAllocation::MemoryAllocation()
			: m_constant_memory(4096, NULL), m_stack_allocation{
		utility::allocator::StackAllocator(4096, NULL),
		utility::allocator::StackAllocator(4096, NULL),
		utility::allocator::StackAllocator(4096, NULL)
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