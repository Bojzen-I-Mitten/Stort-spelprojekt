#include "ProfileManager.h"



namespace thomas 
{
	std::map<const char, ProfileManager::Sample> ProfileManager::s_samples;


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
			s_samples[*name].type;
		}
	}
	void ProfileManager::newFrame()
	{
		// Remove all stamps, but not the actual pair, since it probably
		// be used in the comming frame

		for (auto& element : s_samples)
			element.second.stamps.clear();
	}
}