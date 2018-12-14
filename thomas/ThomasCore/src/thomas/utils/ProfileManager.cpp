#include "ProfileManager.h"
#include <fstream>


namespace thomas 
{
	namespace utils
	{
		namespace profiling
		{
			std::map<std::string, std::vector<float>> ProfileManager::s_textureData;
			std::vector<long long> ProfileManager::s_fps;
			std::map<std::string, std::map<std::string, std::vector<ProfileManager::Stamp>>> ProfileManager::s_samples;
			std::vector<long long> ProfileManager::s_gpuSamples;
			float ProfileManager::s_ramusage;
			std::map<std::string, std::vector<float>> ProfileManager::s_vramusage;
			std::map<std::string, float> ProfileManager::s_totalvramusage;
			float ProfileManager::s_vramTotal;
			unsigned int ProfileManager::s_frames;
			std::atomic<unsigned int> ProfileManager::s_contextSwitch;
			utils::atomics::SpinLock ProfileManager::s_profileLock;
			void ProfileManager::resetFrameCounter()
			{
				s_frames = 0;
				s_contextSwitch = 0;
			}

			void ProfileManager::addTextureData(std::string name, float usage)
			{
				s_profileLock.lock();
				s_textureData[name].push_back(usage);
				s_profileLock.unlock();
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
#ifdef PERFORMANCE
				s_samples[std::to_string((int)processor_id)][name].push_back(std::move(Stamp(elapsedTime, startTime, s_frames)));
#endif 
			}

			void ProfileManager::storeGpuSample(long long gpuTime)
			{
#ifdef PERFORMANCE
				if (gpuTime < 0)
					s_gpuSamples.push_back(0);

				s_gpuSamples.push_back(gpuTime);
#endif 
			}

			void ProfileManager::storeVramSample(std::string name, float usage)
			{
#ifdef MEMORY
				/*s_profileLock.lock();
				s_vramusage[name].push_back(usage);
				s_profileLock.unlock();*/
#endif
			}

			void ProfileManager::increaseContextSwitches()
			{
				s_contextSwitch++;
			}

			std::map<std::string, float> ProfileManager::getVramMap()
			{
				return s_totalvramusage;
			}

			float ProfileManager::lastTextureVramUsage()
			{
				return s_vramusage["Texture2D"].back();
			}

			std::map<std::string, std::vector<float>> ProfileManager::getTextureDetails()
			{
				return s_textureData;
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
				j["SlowfilerData"]["build"]["vramTotal"] = s_vramTotal;
				j["SlowfilerData"]["build"]["vramUsage"];
				//for (auto& id : s_vramusage)
				//{
				//	for (auto& s : id.second)
				//	{
				//		j["SlowfilerData"]["build"][id.first] += std::to_string(s) + ',';
				//	}
				//}

				
				j["SlowfilerData"]["build"]["contextSwitches"] = s_contextSwitch.load();

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
#ifdef MEMORY
				s_ramusage = (usage - s_vramTotal);
#endif
			}
			float ProfileManager::getRAMUsage()
			{
#ifdef MEMORY
				return s_ramusage - s_vramTotal;
#else
				return 0.0f;
#endif
			}

			void ProfileManager::setVRAMUsage(float total)
			{
#ifdef MEMORY
				s_vramTotal = total;
#endif
			}

		}
	}
}