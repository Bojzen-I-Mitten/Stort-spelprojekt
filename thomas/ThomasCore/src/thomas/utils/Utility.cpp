#include "Utility.h"
#include <assert.h>

namespace thomas {
	namespace utility {

		/* Djb2 hash function
		*/
		uint32_t hash(const char *str)
		{
			uint32_t value = 5381;
			int c;

			while (c = *str++)
				value = ((value << 5) + value) + c; /* hash * 33 + c */

			return value;
		}
		/* Djb2 hash function (Safe'r' version)
		*/
		uint32_t hash(const char *str, uint32_t len)
		{
			assert(len < UCHAR_MAX);	// Use for readable strings..!
#ifdef _DEBUG
			uint32_t h_ver = hash(str);	// Hash verification, should always be performed so no corrupt strings are passed and generate different result
#endif
			uint32_t value = 5381;
			int c;
			int step = 0;
			while ((c = *str++) && step++ <= len)
				value = ((value << 5) + value) + c; /* hash * 33 + c */

#ifdef _DEBUG
			assert(h_ver == value);
#endif
			return value;
		}
		uint32_t hash(const std::string &str) {
			return hash(str.c_str(), str.length());
		}
	}
}