#include "MemoryAllocation.h"


namespace thomas {

	namespace resource {

		constexpr uint32_t CONSTANT_MEM = 4096;
		constexpr uint32_t STACK_ALLOC = 4096*2;

		MemoryAllocation::MemoryAllocation()
			: 
			m_allocation(new uint8_t[CONSTANT_MEM + STACK_ALLOC * 3]),
			m_constant_memory(CONSTANT_MEM, m_allocation),
			m_stack_allocation{
		utility::allocator::StackAllocator(STACK_ALLOC, m_allocation + CONSTANT_MEM),
		utility::allocator::StackAllocator(STACK_ALLOC, m_allocation + CONSTANT_MEM + STACK_ALLOC),
		utility::allocator::StackAllocator(STACK_ALLOC, m_allocation + CONSTANT_MEM + STACK_ALLOC * 2)
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