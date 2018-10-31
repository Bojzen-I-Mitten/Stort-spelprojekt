#include "ProfileManager.h"
#include <fstream>


#include "..\..\..\include\nlohmann\json.hpp"
#include "..\..\..\include\imgui\imgui.h"
namespace thomas 
{
	namespace utils
	{

		namespace profiling
		{
			using namespace nlohmann;
			std::map<std::string, std::map<std::string, std::vector<float>>> ProfileManager::s_samples;
			float ProfileManager::ramusage;
			float ProfileManager::vramusage;

			void ProfileManager::storeSample(std::string name, long elapsedTime, DWORD processor_id)
			{
				s_samples[std::to_string((int)processor_id)][name].push_back(elapsedTime);
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
				j["SlowfilerData"]["processor"];
				for (auto& it : s_samples)
					j["SlowfilerData"]["processor"][it.first];

				j["SlowfilerData"]["build"]["ramUsage"] = ramusage;
				j["SlowfilerData"]["build"]["vramUsage"] = vramusage;
				for (auto& id : s_samples)
				{
					for (auto& processor : id.second)
					{
						j["SlowfilerData"]["processor"][id.first]["functions"];
						j["SlowfilerData"]["processor"][id.first]["functions"][processor.first] = processor.second;
					}
				}

				std::ofstream o("data.json");
				o << j << std::endl;
				o.close();
			}

			void ProfileManager::SetRAMUsage(float usage)
			{
				ramusage = usage;
			}

			void ProfileManager::SetVRAMUsage(float usage)
			{
				vramusage = usage;
			}

		}
	}
}