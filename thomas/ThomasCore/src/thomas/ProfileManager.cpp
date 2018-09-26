#include "ProfileManager.h"
#include "ProfileManager.h"
#include <fstream>

namespace thomas 
{
	int ProfileManager::nrOfFrames;
	std::map<const char, ProfileManager::Sample> ProfileManager::s_samples;
	std::vector<std::map<const char, ProfileManager::Sample>> ProfileManager::s_frames;

	void ProfileManager::storeSample(const char* name, long elapsedTime, operationType type)
	{
		// Object exists
		if (s_samples.find(*name) != s_samples.end())
		{
			s_samples[*name].stamps.push_back(elapsedTime);
			s_samples[*name].type = type;
		}
		else // Object does not exists
		{
			s_samples[*name] = Sample(); // Det här är dumt, men kompilatorn är smart.
			s_samples[*name].stamps.push_back(elapsedTime);
			s_samples[*name].name = name;
			s_samples[*name].type;
		}
	}

	void ProfileManager::newFrame()
	{
		nrOfFrames++;
		s_frames.push_back(s_samples);
		s_samples.clear();
		//for (auto& element : s_samples)
		//	element.second.stamps.clear();
	}

	void ProfileManager::dumpDataToFile(const char * filename)
	{
		const char* delimiter = ",";
		std::fstream file;
		file.open(filename, std::ios::out);

		if (file.is_open())
		{
			for (const auto& frame : s_frames)
			{
				for (const auto& keyValuePair : frame)
				{
					file << keyValuePair.second.name << delimiter;
					for (const auto& sample : keyValuePair.second.stamps)
					{
						file << sample << delimiter;
					}
					file << "\n";
				}
				file.close();
			}
		}
	}
}