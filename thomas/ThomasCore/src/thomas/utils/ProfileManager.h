#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <Windows.h>
#include <chrono>
#include <atomic>
#include "..\..\..\include\nlohmann\json.hpp"
#include "..\utils\atomic\Synchronization.h"
#define BENCHMARK
//#undef BENCHMARK

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
				static std::map<std::string, std::vector<float>> s_textureData;
				static std::vector<long long> s_fps;
				static std::map<std::string, std::map<std::string, std::vector<Stamp>>> s_samples;
				static std::vector<long long> s_gpuSamples;
				static float s_ramusage;
				static std::map<std::string, float> s_totalvramusage;
				static std::map<std::string, std::vector<float>> s_vramusage;
				static float s_vramTotal;
				static unsigned int s_frames;
				static std::atomic<unsigned int> s_contextSwitch;
				static utils::atomics::SpinLock s_profileLock;

			public:
				static void addTextureData(std::string name, float usage);
				static void newFrame();
				static void resetFrameCounter();
				static void dumpDataToFile();
				static void storeSample(std::string functionName, long long elapsedTime, long long startTime, DWORD processor_id);
				static void storeGpuSample(long long gpuTime);
				static void storeVramSample(std::string name, float usage);
				static void increaseContextSwitches();
				static std::map<std::string, float> getVramMap();
				static float lastTextureVramUsage();
				static std::map<std::string, std::vector<float>> getTextureDetails();
				static void setRAMUsage(float usage);
				static float getRAMUsage();
				static void setVRAMUsage(float total);
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