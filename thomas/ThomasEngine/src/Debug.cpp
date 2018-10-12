
#include <thomas\ThomasCore.h>
#include "Debug.h"

#pragma managed

void ThomasEngine::Debug::UpdateCoreLog()
{
	std::vector<std::string> nativeOutputs = thomas::ThomasCore::GetLogOutput();
	for (int i = 0; i < nativeOutputs.size(); i++) {
		String^ output = gcnew String(nativeOutputs.at(i).c_str());
		LogCore(output);
	}
	thomas::ThomasCore::ClearLogOutput();
}
