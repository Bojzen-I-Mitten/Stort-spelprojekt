#pragma unmanaged
#include <thomas\ThomasCore.h>
#pragma managed
#include "Debug.h"

void ThomasEngine::Debug::UpdateCoreLog()
{
	std::vector<std::string> nativeOutputs = thomas::ThomasCore::GetLogOutput();
	for (int i = 0; i < nativeOutputs.size(); i++) {
		String^ output = gcnew String(nativeOutputs.at(i).c_str());
		LogCore(output);
	}
	thomas::ThomasCore::ClearLogOutput();
}
