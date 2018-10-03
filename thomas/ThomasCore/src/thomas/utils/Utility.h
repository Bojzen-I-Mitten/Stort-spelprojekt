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
	}
}