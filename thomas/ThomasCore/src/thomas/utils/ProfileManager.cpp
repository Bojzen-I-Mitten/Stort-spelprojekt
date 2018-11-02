#include "ProfileManager.h"
#include <fstream>


namespace thomas 
{
	namespace utils
	{

		namespace profiling
		{
			std::map<std::string, std::map<std::string, std::vector<ProfileManager::Stamp>>> ProfileManager::s_samples;
			float ProfileManager::s_ramusage;
			float ProfileManager::s_vramusage;
			unsigned int ProfileManager::s_frames;

			void ProfileManager::resetFrameCounter()
			{
				s_frames = 0;
			}

			void ProfileManager::newFrame()
			{
				s_frames++;
			}

			void ProfileManager::storeSample(std::string name, long long elapsedTime, long long startTime, DWORD processor_id)
			{
		
				s_samples[std::to_string((int)processor_id)][name].push_back(std::move(Stamp(elapsedTime, startTime, s_frames)));
			}

			//void ProfileManager::storeSample(const char* name, long elapsedTime, DWORD processor_id)
			//{
			//	s_samples[std::to_string((int)processor_id)][name].push_back(elapsedTime);
			//}

			void ProfileManager::dumpDataToFile()
			{
				json j;
				j["SlowfilerData"];
				j["SlowfilerData"]["build"];
				j["SlowfilerData"]["timeline"];
				j["SlowfilerData"]["timeline"]["processor"];

				j["SlowfilerData"]["processor"];

				for (auto& id : s_samples)
				{
					for (auto& processor : id.second)
					{
						j["SlowfilerData"]["processor"][id.first]["functions"];
						j["SlowfilerData"]["processor"][id.first]["functions"][processor.first];

						std::vector<long long> temp_vector;
						for (auto& sample : processor.second)
						{
							temp_vector.push_back(sample.m_duration);
						}

						for (auto& sample : processor.second)
						{
							j["SlowfilerData"]["processor"][id.first]["functions"][processor.first] = temp_vector;
						}
					}
				}

				for (auto& it : s_samples)
					j["SlowfilerData"]["timeline"]["processor"][it.first];

				j["SlowfilerData"]["build"]["ramUsage"] = s_ramusage;
				j["SlowfilerData"]["build"]["vramUsage"] = s_vramusage;

				for (auto& id : s_samples)
				{
					for (auto& processor : id.second)
					{
						j["SlowfilerData"]["timeline"]["processor"][id.first]["functions"];
						for (auto& sample : processor.second)
						{
							j["SlowfilerData"]["timeline"]["processor"][id.first]["functions"][std::to_string(sample.m_frame)][processor.first];
							j["SlowfilerData"]["timeline"]["processor"][id.first]["functions"][std::to_string(sample.m_frame)][processor.first]["duration"] = sample.m_duration;
							j["SlowfilerData"]["timeline"]["processor"][id.first]["functions"][std::to_string(sample.m_frame)][processor.first]["startTime"] = sample.m_startTime;
						}
					}
				}

				std::ofstream o("data.json");
				o << j << std::endl;
				o.close();
			}

			void ProfileManager::setRAMUsage(float usage)
			{
				s_ramusage = usage;
			}

			void ProfileManager::setVRAMUsage(float usage)
			{
				s_vramusage = usage;
			}

		}
	}
}