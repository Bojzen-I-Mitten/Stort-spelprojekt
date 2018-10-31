#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <Windows.h>

#define BENCHMARK


namespace thomas
{
	namespace utils
	{
		namespace profiling
		{

			class ProfileManager
			{
			public:

			private:
				static std::map<std::string, std::map<std::string, std::vector<float>>> s_samples;
				static float ramusage;
				static float vramusage;

			public:
				static void dumpDataToFile();
				static void storeSample(std::string functionName, long elapsedTime, DWORD processor_id);
				static void SetRAMUsage(float usage);
				static void SetVRAMUsage(float usage);
			private:


			};
		}
	}
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::utils::profiling::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif