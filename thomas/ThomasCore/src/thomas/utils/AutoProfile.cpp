#include "AutoProfile.h"

namespace thomas
{
	namespace utils
	{
		namespace profiling
		{
			// std::chrono could be a bit slow for our needs, needs more investigation.
			AutoProfile::AutoProfile(const char* name)
			{
				m_processor_ID = GetCurrentProcessorNumber();
				m_name = name;
				m_startTime = std::chrono::high_resolution_clock::now();
			}

			AutoProfile::~AutoProfile()
			{
				if (m_processor_ID != GetCurrentProcessorNumber())
				{
					// Pipeline will guess false
				}	
				else
				{
					std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
					std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime);

					ProfileManager::storeSample(m_name, duration.count(), m_startTime.time_since_epoch().count(), m_processor_ID);
				}
			}
		}
	}
}