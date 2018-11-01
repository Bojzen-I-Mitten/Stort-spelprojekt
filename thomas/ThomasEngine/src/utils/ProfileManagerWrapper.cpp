#pragma unmanaged 

#pragma managed
#include "ProfileManagerWrapper.h"
#include <msclr\marshal_cppstd.h>


namespace ThomasEngine
{
	void ProfileManagerWrapper::addSample(System::String^ functionNane, long timestamp, long long startTime, int processor_ID)
	{
		msclr::interop::marshal_context context;
		std::string standardString = context.marshal_as<std::string>(functionNane);
		thomas::utils::profiling::ProfileManager::storeSample(standardString, timestamp, startTime, processor_ID);
	}
}