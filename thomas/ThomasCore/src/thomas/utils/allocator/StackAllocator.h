#pragma once
#include "Allocator.h"

namespace thomas {
	namespace utility {
		namespace allocator {

			class StackAllocator : public Allocator
			{
			public:

				StackAllocator(size_t size, void* start);
				~StackAllocator();

				void* allocate(size_t size, uint8_t alignment) override;
				void deallocate(void* p) override;

			private:

				StackAllocator(const StackAllocator&);

				//Prevent copies because it might cause errors 
				StackAllocator& operator=(const StackAllocator&) = delete;

				struct AllocationHeader
				{
#if _DEBUG 
					void* prev_address;
#endif 

					uint8_t adjustment;
				};

#if _DEBUG 
				void* _prev_position;
#endif 

				void* _current_pos;
			};


		}
	}
}