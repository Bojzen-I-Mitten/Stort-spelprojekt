#pragma once
#pragma unmanaged 
#include <thomas/utils/ProfileManager.h>
#include <string>
#pragma managed


namespace ThomasEngine {
	public ref class ProfileManagerWrapper
	{
	public:
		static void addSample(System::String^ functionNane, long timestamp, long long startTime, int processor_ID);
	};
}