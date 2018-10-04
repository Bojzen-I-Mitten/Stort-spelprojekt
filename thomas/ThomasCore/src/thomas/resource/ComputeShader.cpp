#include "ComputeShader.h"
#include "ShaderProperty\shaderProperties.h"

namespace thomas
{
	namespace resource
	{
		
		void ComputeShader::Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			//Bind();
			utils::D3D::Instance()->GetDeviceContext()->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}

		void ComputeShader::DispatchIndirect(ID3D11Buffer* indirectBuffer, unsigned alignedByteOffsetForArgs)
		{
			utils::D3D::Instance()->GetDeviceContext()->DispatchIndirect(indirectBuffer, alignedByteOffsetForArgs);
		}

	}
}