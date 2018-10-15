#pragma once
#include "Allocator.h" 

namespace thomas {
	namespace utility {
		namespace allocator {

			class LinearAllocator : public Allocator
			{
			public:

				LinearAllocator(size_t size, void* start);
				LinearAllocator(LinearAllocator&& move);
				LinearAllocator& operator=(LinearAllocator&& move);
				~LinearAllocator();

				void* allocate(size_t size, uint8_t alignment) override;
				void deallocate(void* p) override;
				void clear();

			private:

				LinearAllocator(const LinearAllocator&) = delete;

				//Prevent copies because it might cause errors 
				LinearAllocator& operator=(const LinearAllocator&) = delete;
				void* _current_pos;
			};
		}
	}
}