#include "ComputeShader.h"

namespace thomas
{
	namespace resource
	{
		
		void ComputeShader::Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			ThomasCore::GetDeviceContext()->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}

	}
}