#pragma once

#include <stdint.h>
#include <string>

namespace thomas {
	namespace utility {

		/* Djb2 hash function
		*/
		uint32_t hash(const char *str);
		/* Djb2 hash function (Safe'r' version)
		*/
		uint32_t hash(const char *str, uint32_t len);
		uint32_t hash(const std::string &str);


		/* Set the flag bits in the value. */
		inline void setFlag(uint32_t &value, uint32_t flag) {
			value |= flag;
		}
		/* Remove the flag bits from the value. */
		inline void rmvFlag(uint32_t &value, uint32_t flag) {
			value &= ~flag;
		}
		/* Check if value matches atleast a single flag. */
		inline bool hasFlag(uint32_t value, uint32_t flag) {
			return value & flag;
		}
		/* Check if value has ALL set all flags specified */
		inline bool matchFlags(uint32_t value, uint32_t flags) {
			return (value & flags) == flags;
		}
		inline void* alignForward(void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* alignForward(const void* address, uint8_t alignment)
		{
			return (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline void* alignBackward(void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline const void* alignBackward(const void* address, uint8_t alignment)
		{
			return (void*)(reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(~(alignment - 1)));
		}

		inline uint8_t alignForwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1));

			if (adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}
		inline uint8_t alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment, uint8_t header_size)
		{
			uint8_t adjustment = alignForwardAdjustment(address, alignment);
			uint8_t needed_space = header_size;
			if(adjustment < needed_space)
			{
				needed_space -= adjustment;
				//Increase adjustment to fit header
				adjustment += alignment * (needed_space / alignment);
				if(needed_space % alignment > 0)
					adjustment += alignment;
			}
			return adjustment;
		}
		
		template<typename T>
		inline uint8_t alignForwardAdjustmentWithHeader(const void* address, uint8_t alignment)
		{
			if (__alignof(T) > alignment)
				alignment = __alignof(T);

			uint8_t adjustment = sizeof(T) + alignForwardAdjustment(pointer_math::add(address, sizeof(T)), alignment);

			return adjustment;
		}

		inline uint8_t alignBackwardAdjustment(const void* address, uint8_t alignment)
		{
			uint8_t adjustment = reinterpret_cast<uintptr_t>(address)& static_cast<uintptr_t>(alignment - 1);

			if (adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}
		inline void* add(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p) + x);
		}

		inline const void* add(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p) + x);
		}

		inline void* subtract(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uintptr_t>(p) - x);
		}

		inline const void* subtract(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uintptr_t>(p) - x);
		}
	}
}