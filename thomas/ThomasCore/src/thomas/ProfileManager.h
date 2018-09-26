#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#define BENCHMARK

namespace thomas
{
	class ProfileManager
	{

	public:
		enum class operationType { rendering, miscLogic, gameObjects, physics, animation };
		struct Sample
		{
			operationType type;
			std::vector<long> stamps;
		};

		static void storeSample(const char* string, long elapsedTime, operationType);
		static void newFrame();
		
	private:
		static std::map<const char, Sample> s_samples;
	};
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif