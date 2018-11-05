#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <Windows.h>

#include "..\..\..\include\nlohmann\json.hpp"
#define BENCHMARK


namespace thomas
{
	namespace utils
	{
		namespace profiling
		{

			using namespace nlohmann;
			class ProfileManager
			{
			public:
				struct Stamp
				{
					unsigned int m_frame;
					long long m_duration;
					long long m_startTime;

					Stamp( long long duration, long long startTime, unsigned int frame)
					{
						m_duration = duration;
						m_startTime = startTime;
						m_frame = frame;
					}
				};

			private:
				static std::map<std::string, std::map<std::string, std::vector<Stamp>>> s_samples;
				static float s_ramusage;
				static float s_vramusage;
				static unsigned int s_frames;

			public:
				static void newFrame();
				static void resetFrameCounter();
				static void dumpDataToFile();
				static void storeSample(std::string functionName, long long elapsedTime, long long startTime, DWORD processor_id);
				static void setRAMUsage(float usage);
				static void setVRAMUsage(float usage);
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