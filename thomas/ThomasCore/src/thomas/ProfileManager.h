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
		enum class operationType { rendering, miscLogic, gameObjects, physics, animation, allocation };
		struct Sample
		{
			const char* name;
			operationType type;
			std::vector<long> stamps;
		};

		static void storeSample(const char* functionName, long elapsedTime, operationType);
		static void newFrame();
		static void dumpDataToFile(const char* filename);

	private:
		static std::map<const char, Sample> s_samples;
		static int nrOfFrames;
	};
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif