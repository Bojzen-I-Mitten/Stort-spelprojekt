#pragma once
#include<memory>
#include "../Utility.h"
namespace thomas {
	namespace utility {
		namespace allocator {

			constexpr uint32_t OWNED_BY_CORE = 0;
			constexpr uint32_t OWNED_BY_ENGINE = 1;	// ThomasEngine maintains ownership of the pointer. And can invalidate without ensuring the pointer is updated.
			/* Thomas Pointer
			*/
			template<typename T>
			struct ThomasPointer
			{
			private:
				T* m_ptr;
				uint32_t m_flags;
				void destroy();
			public:

				ThomasPointer();
				ThomasPointer(T* ptr, uint32_t pointerFlags);
				ThomasPointer(ThomasPointer&& copy);
				ThomasPointer(const ThomasPointer& copy) = delete;
				~ThomasPointer();

				/* Assign the pointer, overwriting if non-empty.
				*/
				void assign(T* ptr, uint32_t pointerFlags);
				/* Invalidate the pointer, deleting if necessary.
				*/
				void unassign();
				/* True if pointer is NOT null.
				*/
				bool empty();

				ThomasPointer& operator=(const ThomasPointer& copy) = delete;
				ThomasPointer& operator=(ThomasPointer&& copy);


				T* operator->() const
				{
					return m_ptr;
				}
			};

			template<typename T>
			inline ThomasPointer<T>::ThomasPointer()
				: m_ptr(nullptr), m_flags(0)
			{
			}

			template<typename T>
			inline ThomasPointer<T>::ThomasPointer(T * ptr, uint32_t pointerFlags)
				: m_ptr(ptr), m_flags(pointerFlags)
			{
			}

			template<typename T>
			inline ThomasPointer<T>::ThomasPointer(ThomasPointer && copy)
				: m_ptr(copy.m_ptr), m_flags(copy.m_flags)
			{
				copy.m_ptr = 0;
			}
			template<typename T>
			inline ThomasPointer<T> & ThomasPointer<T>::operator=(ThomasPointer && copy)
			{
				if (this == &copy) return *this;
				destroy();
				m_ptr = copy.m_ptr;
				m_flags = copy.m_flags;
				copy.m_ptr = 0;
				return *this;
			}

			template<typename T>
			inline ThomasPointer<T>::~ThomasPointer()
			{
				destroy();
			}

			template<typename T>
			inline void ThomasPointer<T>::assign(T * ptr, uint32_t pointerFlags)
			{
				if(m_ptr)
					destroy();
				m_ptr = ptr;
				m_flags = pointerFlags;
			}

			template<typename T>
			inline void ThomasPointer<T>::unassign()
			{
				if (m_ptr)
					destroy();
				m_ptr = nullptr;
				m_flags = 0;
			}

			template<typename T>
			inline bool ThomasPointer<T>::empty()
			{
				return m_ptr == nullptr;
			}

			template<typename T>
			inline void ThomasPointer<T>::destroy()
			{
				// Assure this is not an engine pointer.
				if (utility::noFlag(m_flags, OWNED_BY_ENGINE))
					delete m_ptr;
			}

		}
	}
}