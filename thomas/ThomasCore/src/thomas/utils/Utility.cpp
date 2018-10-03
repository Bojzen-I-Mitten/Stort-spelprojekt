#include "Utility.h"
#include <assert.h>

namespace thomas {
	namespace utility {

		/* Djb2 hash function
		*/
		uint32_t hash(const char *str)
		{
			uint32_t hash = 5381;
			int c;

			while (c = *str++)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
		/* Djb2 hash function (Safe'r' version)
		*/
		uint32_t hash(const char *str, uint32_t len)
		{
			assert(len < UCHAR_MAX);	// Use for readable strings..!
			uint32_t hash = 5381;
			int c;
			int step = 0;
			while (c = *str++ && step++ <= len)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
		uint32_t hash(const std::string &str) {
			return hash(str.c_str(), str.length());
		}
	}
}