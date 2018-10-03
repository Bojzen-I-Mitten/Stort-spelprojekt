#include "Allocator.h"


namespace thomas {
	namespace utility {
		namespace allocator {


			Allocator::Allocator(size_t size, void* start) :
				_start(start),
				_size(size),
				_used_memory(0),
				_num_allocations(0)
			{
			}
			Allocator::Allocator(Allocator && move) :
				_start(move._start),
				_size(move._size),
				_used_memory(move._used_memory),
				_num_allocations(move._num_allocations)
			{				}

		}
	}
}