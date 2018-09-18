#include "ComputeShader.h"

namespace thomas
{
	namespace resource
	{
		ComputeShader::ComputeShader(ID3DX11Effect* effect, std::string path) : Shader(effect, path)
		{
			
		}
		void ComputeShader::Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			//Bind();
			ThomasCore::GetDeviceContext()->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}
		
		
	}
}