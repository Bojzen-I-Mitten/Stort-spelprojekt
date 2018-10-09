#pragma once

#include "../utils/allocator/LinearAllocator.h"
#include "../utils/allocator/StackAllocator.h"

namespace thomas {

	namespace resource {

		constexpr uint32_t NUM_THREAD = 3;

		class MemoryAllocation {
		public:

			MemoryAllocation();
			~MemoryAllocation();

			/* Fetch a stack by index. */
			utility::allocator::StackAllocator& stack(uint32_t index);

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