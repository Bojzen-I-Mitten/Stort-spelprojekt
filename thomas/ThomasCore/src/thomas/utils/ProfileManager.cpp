#include "ProfileManager.h"
#include <fstream>


namespace thomas 
{
	namespace utils
	{
		namespace profiling
		{
			std::vector<long long> ProfileManager::s_fps;
			std::map<std::string, std::map<std::string, std::vector<ProfileManager::Stamp>>> ProfileManager::s_samples;
			std::vector<long long> ProfileManager::s_gpuSamples;
			float ProfileManager::s_ramusage;
			std::map<std::string, std::vector<float>> ProfileManager::s_vramusage;
			float ProfileManager::s_vrambudget;
			unsigned int ProfileManager::s_frames;

			void ProfileManager::resetFrameCounter()
			{
				s_frames = 0;
			}

			void ProfileManager::newFrame()
			{
				// Init to starting point, will produce a frametime of zero
				static std::chrono::steady_clock::time_point tracktime = std::chrono::high_resolution_clock::now();

				std::chrono::nanoseconds frametime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - tracktime);
				
				tracktime = std::chrono::high_resolution_clock::now();

				s_fps.push_back(frametime.count());

				s_frames++;
			}

			void ProfileManager::storeSample(std::string name, long long elapsedTime, long long startTime, DWORD processor_id)
			{
				s_samples[std::to_string((int)processor_id)][name].push_back(std::move(Stamp(elapsedTime, startTime, s_frames)));
			}

			void ProfileManager::storeGpuSample(long long gpuTime)
			{
				if (gpuTime < 0)
					s_gpuSamples.push_back(0);

				s_gpuSamples.push_back(gpuTime);
			}

			void ProfileManager::storeVramSample(std::string name, float usage)
			{
				s_vramusage[name].push_back(usage);
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

				j["SlowfilerData"]["build"]["fps"] = s_fps;

				j["SlowfilerData"]["build"]["gpu"] = s_gpuSamples;

				j["SlowfilerData"]["processor"];

				for (auto& id : s_samples)
				{
					std::string temp;
					temp.reserve(100000);
					for (auto& processor : id.second)
					{
						j["SlowfilerData"]["processor"][id.first]["functions"];
						j["SlowfilerData"]["processor"][id.first]["functions"][processor.first];

						std::vector<long long> temp_vector;

						
						for (int i = 0; i < processor.second.size() - 1; i++) 
						{
							temp += ' ' + std::to_string(processor.second[i].m_duration) + ',';
						}
						temp += std::to_string(processor.second.back().m_duration);
						auto& obj = j["SlowfilerData"]["processor"][id.first]["functions"][processor.first];
						for (auto& sample : processor.second)
						{
							 obj = temp;
						}
						temp.clear();
					}
				}


				j["SlowfilerData"]["build"]["ramUsage"] = s_ramusage;
				j["SlowfilerData"]["build"]["vramUsage"] = s_vramusage;
				/*

				j["SlowfilerData"]["timeline"];
				j["SlowfilerData"]["timeline"]["processor"];


				for (auto& it : s_samples)
					j["SlowfilerData"]["timeline"]["processor"][it.first];

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
				*/
				std::ofstream o("data.json");
				o << j << std::endl;
				o.close();
			}

			void ProfileManager::setRAMUsage(float usage)
			{
				s_ramusage = usage;
			}
			float ProfileManager::getRAMUsage()
			{
				return s_ramusage;
			}

			void ProfileManager::setVRAMUsage(float usage, float budget)
			{
				s_vrambudget = budget;
			}

		}
	}
}