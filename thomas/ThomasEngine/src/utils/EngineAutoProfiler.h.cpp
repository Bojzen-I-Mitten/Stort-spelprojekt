#include "EngineAutoProfiler.h.h"
#include "ProfileManagerWrapper.h"
namespace ThomasEngine
{
	EngineAutoProfiler::EngineAutoProfiler(System::String^ functionName) 
	{
		m_functionName = functionName;
		m_processor_ID = GetCurrentProcessorNumber();
		m_timestamp.Start();
	}
	EngineAutoProfiler::~EngineAutoProfiler()
	{

	}
	void EngineAutoProfiler::sendSample()
	{
		m_timestamp.Stop();
		if (m_processor_ID != GetCurrentProcessorNumber())
		{
			ProfileManagerWrapper::addSample(m_functionName, m_timestamp.ElapsedMilliseconds, m_processor_ID);
		}
		else
		{
			ProfileManagerWrapper::addSample(m_functionName, m_timestamp.ElapsedMilliseconds, m_processor_ID);
		}
	}
}
