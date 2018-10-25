#include "ProfileManager.h"
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
			std::map<const char*, std::vector<float>> ProfileManager::s_samples;

			void ProfileManager::storeSample(const char* name, long elapsedTime)
			{
				s_samples[name].push_back(elapsedTime);
			}

			void ProfileManager::newFrame()
			{
				
			}

			void ProfileManager::dumpDataToFile()
			{
				json j;
				j["SlowfilerData"];
				j["SlowfilerData"]["functions"];
				for (auto& it : s_samples)
				{
					j["SlowfilerData"]["functions"][it.first];
					j["SlowfilerData"]["functions"][it.first]["Sample"] = it.second;
				}

				std::ofstream o("data.json");
				o << j << std::endl;
				o.close();
			}

			void ProfileManager::DisplaySample(const char * functionName, long elapsedTime)
			{
				// Display using Imgui
				// And store sample
				bool test = true;


				storeSample(functionName, elapsedTime);
			}

			long ProfileManager::GetLatestStamp(const char * functionName)
			{
				if (s_samples.find(functionName) != s_samples.end())
				{
					return 0;
				}

				return s_samples[functionName].back();
			}

			std::map<const char*, std::vector<float>>* ProfileManager::GetData()
			{
				return &s_samples;
			}
		}
	}
}