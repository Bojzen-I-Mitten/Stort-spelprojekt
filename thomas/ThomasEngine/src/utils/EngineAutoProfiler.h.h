#pragma once
#pragma unmanaged 
#include <string>
#include <chrono>
#pragma managed
namespace ThomasEngine {
	public ref class EngineAutoProfiler
	{
	public:
		EngineAutoProfiler(System::String^  functionName);
		~EngineAutoProfiler();
		void sendSample();
	private:
		int m_processor_ID;
		System::String^ m_functionName;
		System::Diagnostics::Stopwatch m_timestamp;
	};
}