#pragma once

#include "../utils/allocator/LinearAllocator.h"
#include "../utils/allocator/StackAllocator.h"
#include "../Constants.h"
namespace thomas {

	namespace resource {
		
		class MemoryAllocation {
		public:

			MemoryAllocation();
			~MemoryAllocation();

			/* Fetch a stack by index. 
			*/
			utility::allocator::StackAllocator& stack(uint32_t index);

			void* allocSystemMemory(uint32_t bytes, uint32_t alignment);

		private:

			/* Allocated system memory.
			*/
			uint8_t * m_allocation;
			/* Constant memory (ish, not determined use case).
			*/
			utility::allocator::LinearAllocator m_constant_memory;
			/* Thread stack allocations
			*/
			utility::allocator::StackAllocator m_stack_allocation[NUM_THREAD];
		};
	}
}