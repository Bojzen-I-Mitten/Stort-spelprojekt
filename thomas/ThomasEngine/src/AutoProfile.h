#pragma once
#include <chrono>
#include "ProfileManager.h"

namespace Thomas
{
	class AutoProfile
	{
	public:
		AutoProfile(const char* name, ProfileManager::operationType type);
		~AutoProfile();

		const char* m_name;
		ProfileManager::operationType m_type;
		std::chrono::steady_clock::time_point m_startTime;

	};
}