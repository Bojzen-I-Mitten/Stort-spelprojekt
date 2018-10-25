#include "ProfileManager.h"
#include "ProfileManager.h"
#include <fstream>

namespace thomas 
{
	namespace utils
	{

		namespace profiling
		{
			int ProfileManager::nrOfFrames;
			std::map<const char*, ProfileManager::Sample> ProfileManager::s_samples;

			void ProfileManager::storeSample(const char* name, long elapsedTime, operationType type)
			{
				// Object exists
				if (s_samples.find(name) != s_samples.end())
				{
					s_samples[name].stamps.reserve(10000);
					s_samples[name].stamps.push_back(std::move(Stamp(elapsedTime, nrOfFrames)));
				}
				else // Object does not exists
				{
					s_samples[name] = std::move(Sample(name, type, elapsedTime, nrOfFrames)); // moveable
				}
			}

			void ProfileManager::newFrame()
			{
				nrOfFrames++;
			}

			void ProfileManager::dumpDataToFile(const char * filename)
			{
				const char* delimiter = " ";
				std::fstream file;
				file.open(filename, std::ios::out);

				if (file.is_open())
				{
					for (const auto& keyValuePair : s_samples)
					{
						file << keyValuePair.second.name << delimiter;
						for (const auto& sample : keyValuePair.second.stamps)
						{
							file << sample.timestamp << delimiter;
						}
						file << "\n";
					}
					file.close();
				}
			}
		}
	}
}