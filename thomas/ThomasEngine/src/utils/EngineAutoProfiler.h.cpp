#include "EngineAutoProfiler.h.h"
#include "ProfileManagerWrapper.h"
namespace ThomasEngine
{
	EngineAutoProfiler::EngineAutoProfiler(System::String^ functionName) 
	{
		m_functionName = functionName;
		m_processor_ID = GetCurrentProcessorNumber();
		m_timestamp.Start();
		m_startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	}
	EngineAutoProfiler::~EngineAutoProfiler()
	{

	}
	void EngineAutoProfiler::sendSample()
	{
		m_timestamp.Stop();
		if (m_processor_ID != GetCurrentProcessorNumber())
		{
			
		}
		else
		{
			ProfileManagerWrapper::addSample(m_functionName, m_timestamp.ElapsedMilliseconds, m_startTime, m_processor_ID);
		}
	}
}
