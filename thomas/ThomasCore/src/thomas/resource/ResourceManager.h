#pragma once

#include "../utils/allocator/LinearAllocator.h"

namespace thomas {

	namespace resource {

		class ResourceManager {

			utility::allocator::LinearAllocator m_alloc;

			ResourceManager();
			~ResourceManager();
		};
	}
}