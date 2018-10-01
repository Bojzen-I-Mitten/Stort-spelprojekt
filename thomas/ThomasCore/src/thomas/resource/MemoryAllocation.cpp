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

	}
}