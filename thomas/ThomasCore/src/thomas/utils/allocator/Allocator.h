#pragma once
#include <assert.h>
#include <stdint.h>

// Interface and inherited classes made by: Tiago Costa
// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/c-custom-memory-allocation-r3010/
// https://github.com/tiagovcosta/aquaengine/tree/master/AquaEngine/Core/Allocators

namespace thomas {
	namespace utility {
		namespace allocator {

			class Allocator
			{
			public:

				template<class T>
				struct AllocT
				{
					Allocator& m_alloc;
					T* m_ptr;
					AllocT(Allocator& alloc, T* data);
					~AllocT();

					AllocT(const AllocT& copy) = delete;
					AllocT& operator=(const AllocT& other) = delete;
					AllocT(AllocT&& move);
					AllocT& operator=(AllocT&& other);

					T& operator[](uint32_t index) { return m_ptr[index]; }
				};

				Allocator(size_t size, void* start);
				Allocator(Allocator && move);


				virtual ~Allocator()
				{
					assert(_num_allocations == 0 && _used_memory == 0);
					_start = nullptr; _size = 0;
				}

				virtual void* allocate(size_t size, uint8_t alignment) = 0;
				virtual void deallocate(void* p) = 0;
				
				template<class T>
				AllocT<T> allocateArr(size_t size, uint8_t alignment = alignof(T));


				void* getStart() const { return _start; }
				size_t getSize() const { return _size; }
				size_t getUsedMemory() const { return _used_memory; }
				size_t getNumAllocations() const { return _num_allocations; }

			protected:
				/* Move operation
				*/
				void moveOp(Allocator&& move);

				void* _start;
				size_t _size;
				size_t _used_memory;
				size_t _num_allocations;
			};


			namespace allocator
			{
				template <class T> T* allocateNew(Allocator& allocator)
				{
					return new (allocator.allocate(sizeof(T), __alignof(T))) T;
				}

				template <class T> T* allocateNew(Allocator& allocator, const T& t)
				{
					return new (allocator.allocate(sizeof(T), __alignof(T))) T(t);
				}

				template <class T> void deallocateDelete(Allocator& allocator, T& object)
				{
					object.~T();
					allocator.deallocate(&object);
				}

				template <class T> T* allocateArray(Allocator& allocator, size_t length)
				{
					assert(length != 0);
					u8 headerSize = sizeof(size_t) / sizeof(T);

					if (sizeof(size_t) % sizeof(T) > 0) headerSize += 1;

					//Allocate extra space to store array length in the bytes before the array 
					T* p = ((T*)allocator.allocate(sizeof(T)*(length + headerSize), __alignof(T))) + headerSize;
					*(((size_t*)p) - 1) = length;

					for (size_t i = 0; i < length; i++)
						new (&p) T;

					return p;
				}

				template <class T> void deallocateArray(Allocator& allocator, T* array)
				{
					assert(array != nullptr);
					size_t length = *(((size_t*)array) - 1);

					for (size_t i = 0; i < length; i++) array.~T();

					//Calculate how much extra memory was allocated to store the length before the array 
					u8 headerSize = sizeof(size_t) / sizeof(T);
					if (sizeof(size_t) % sizeof(T) > 0)
						headerSize += 1;
					allocator.deallocate(array - headerSize);
				}
			};

			template<class T>
			Allocator::AllocT<T> Allocator::allocateArr(size_t num_elem, uint8_t alignment)
			{
				return AllocT<T>(*this, allocate(num_elem * sizeof(T), alignment));
			}
			template<class T>
			inline Allocator::AllocT<T>::AllocT(Allocator& alloc, T* data)
				: m_alloc(alloc), m_ptr(data)
			{
			}
			template<class T>
			inline Allocator::AllocT<T>::~AllocT()
			{
				m_alloc.deallocate(m_ptr);
			}
			template<class T>
			inline Allocator::AllocT<T>::AllocT(AllocT && move)
				: m_alloc(move.m_alloc), m_ptr(move.m_ptr)
			{
				move.m_ptr = nullptr;
			}
			template<class T>
			inline Allocator::AllocT<T>& Allocator::AllocT<T>::operator=(AllocT && other)
			{
				if (this == &other) return *this;
				this->m_alloc = move.m_alloc;
				this->m_ptr = move.m_ptr;
				move.m_ptr = nullptr;
				return *this;
			}
}
	}
}