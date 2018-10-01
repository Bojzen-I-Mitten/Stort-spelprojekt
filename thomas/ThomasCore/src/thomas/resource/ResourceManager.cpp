#include "ResourceManager.h"


namespace thomas {

	namespace resource {



		ResourceManager::ResourceManager()
			: constant_memory(4096, NULL)
		{
		}

		ResourceManager::~ResourceManager()
		{
		}

	}
}