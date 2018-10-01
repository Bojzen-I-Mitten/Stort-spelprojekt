#pragma once

#include "../utils/allocator/LinearAllocator.h"
#include "../utils/allocator/StackAllocator.h"

namespace thomas {

	namespace resource {

		class ResourceManager {

			utility::allocator::LinearAllocator constant_memory;
			utility::allocator::StackAllocator stack_allocation;

			ResourceManager();
			~ResourceManager();
		};
	}
}