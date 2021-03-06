#include "StackAllocator.h" 
#include "../Utility.h"

namespace thomas {
	namespace utility {
		namespace allocator {

			StackAllocator::StackAllocator(size_t size, void* start) 
				: Allocator(size, start), _current_pos(start)
			{
				assert(size > 0);
#if _DEBUG 
				_prev_position = nullptr;
#endif 
			}


			StackAllocator::StackAllocator(StackAllocator&& move)
				: Allocator(std::move(move)), _current_pos(move._current_pos)
#if _DEBUG 
				,_prev_position(move._prev_position)
#endif 
			{
			}

			StackAllocator::~StackAllocator()
			{
#if _DEBUG 
				_prev_position = nullptr;
#endif 

				_current_pos = nullptr;
			}

			void* StackAllocator::allocate(size_t size, uint8_t alignment)
			{
				assert(size != 0);
				uint8_t adjustment = alignForwardAdjustmentWithHeader(_current_pos, alignment, sizeof(AllocationHeader));

				if (_used_memory + adjustment + size > _size)
				{
					assert(false);
					return nullptr;
				}
				void* aligned_address = add(_current_pos, adjustment);

				//Add Allocation Header 
				AllocationHeader* header = (AllocationHeader*)(subtract(aligned_address, sizeof(AllocationHeader)));
				header->adjustment = adjustment;

#if _DEBUG 
				header->prev_address = _prev_position;
				_prev_position = aligned_address;
#endif 

				_current_pos = add(aligned_address, size);
				_used_memory += size + adjustment;
				_num_allocations++;

				return aligned_address;
			}

			void StackAllocator::deallocate(void* p)
			{
				if (!p) return; //Assert null
#if _DEBUG 
				assert(p == _prev_position);	// Non-deleted stack data.
#endif 

				//Access the AllocationHeader in the bytes before p 
				AllocationHeader* header = (AllocationHeader*)(subtract(p, sizeof(AllocationHeader)));
				_used_memory -= (uintptr_t)_current_pos - (uintptr_t)p + header->adjustment;
				_current_pos = subtract(p, header->adjustment);

#if _DEBUG 
				_prev_position = header->prev_address;
#endif 

				_num_allocations--;
			}
		}
	}
}