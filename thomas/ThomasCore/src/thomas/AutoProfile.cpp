#include "AutoProfile.h"

namespace thomas
{
	// std::chrono could be a bit slow for our needs, needs more investigation.
	AutoProfile::AutoProfile(const char* name, ProfileManager::operationType type)
	{
		m_name = name;
		m_startTime = std::chrono::high_resolution_clock::now();
	}

	AutoProfile::~AutoProfile()
	{
		std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds stamp = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime);

		ProfileManager::storeSample(m_name, stamp.count(), m_type);
	}
}