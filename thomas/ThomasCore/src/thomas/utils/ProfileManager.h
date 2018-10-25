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
				enum class operationType { rendering, miscLogic, gameObjects, physics, animation, allocation };


				struct Stamp
				{
					long timestamp;
					unsigned int frame;
					Stamp(long timestamp, unsigned int frame) : timestamp(timestamp), frame(frame)
					{

					};
				};
				struct Sample
				{
					const char* name;
					operationType type;
					std::vector<Stamp> stamps;

					Sample(const char* name, operationType type, long timestamp, unsigned frame) :
						name(name), type(type)
					{
						stamps.push_back(std::move(Stamp(timestamp, frame)));
					}

					Sample() {};
				};


				static void storeSample(const char* functionName, long elapsedTime, operationType);
				static void newFrame();
				static void dumpDataToFile(const char* filename);

			private:
				static std::map<const char*, Sample> s_samples;
				static int nrOfFrames;

			};
		}
	}
}



#ifdef BENCHMARK
#define NEW_FRAME() thomas::utils::profiling::ProfileManager::newFrame();
#else
#define NEW_FRAME() 
#endif