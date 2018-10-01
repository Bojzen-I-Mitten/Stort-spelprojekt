#pragma once

#include "../utils/allocator/LinearAllocator.h"
#include "../utils/allocator/StackAllocator.h"

namespace thomas {

	namespace resource {

		constexpr uint32_t NUM_THREAD = 3;

		class MemoryAllocation {

			utility::allocator::LinearAllocator m_constant_memory;
			utility::allocator::StackAllocator m_stack_allocation[NUM_THREAD];

			MemoryAllocation();
			~MemoryAllocation();
		};
	}
}