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
	}
}