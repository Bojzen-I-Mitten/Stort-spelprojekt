#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>


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
				static std::map<const char*, std::vector<float>> s_samples;

			public:
				static void newFrame();
				static void dumpDataToFile();
				static void DisplaySample(const char* functionName, long elapsedTime);
				static long GetLatestStamp(const char* functionName);
				static std::map<const char*, std::vector<float>>* GetData();
			private:


				static void storeSample(const char* functionName, long elapsedTime);

			};
		}
	}
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::utils::profiling::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif