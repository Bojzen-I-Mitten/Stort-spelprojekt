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
				static std::map<std::string, std::map<const char*, std::vector<float>>> s_samples;
				static float ramusage;
				static float vramusage;

			public:
				static void newFrame();
				static void dumpDataToFile();
				static void DisplaySample(const char* functionName, long elapsedTime, DWORD processor_id);
				static void SetRAMUsage(float usage);
				static void SetVRAMUsage(float usage);
			private:


				static void storeSample(const char* functionName, long elapsedTime, DWORD processor_id);

			};
		}
	}
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::utils::profiling::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif