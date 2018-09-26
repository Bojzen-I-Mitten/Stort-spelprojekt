#pragma once
#include <chrono>
#include "ProfileManager.h"



namespace thomas
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

#ifdef BENCHMARK
#define PROFILE(name, type) AutoProfile temp = AutoProfile(name, type);
#else
#define PROFILE(name, type) 
#endif