#pragma once
#include <chrono>
#include <Windows.h>
#include "ProfileManager.h"



namespace thomas
{
	namespace utils
	{
		enum class type
		{
			render, logic, animation, light
		};
		namespace profiling
		{
			class AutoProfile
			{
			public:
				AutoProfile(const char* name);
				~AutoProfile();

			private:
				DWORD m_processor_ID;
				const char* m_name;
				std::chrono::steady_clock::time_point m_startTime;

			};
		}
	}
}
#ifdef BENCHMARK
#define PROFILE(name) thomas::utils::profiling::AutoProfile temp = thomas::utils::profiling::AutoProfile(name);
#else
#define PROFILE(name) 
#endif