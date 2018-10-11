#include "ComputeShader.h"
#include "ShaderProperty\shaderProperties.h"

namespace thomas
{
	namespace resource
	{
		ID3D11UnorderedAccessView* const ComputeShader::s_nullUAVs[8]  = { NULL };
		ID3D11UnorderedAccessView* const ComputeShader::s_nullUAV[1]   = { NULL };
		ID3D11ShaderResourceView*  const ComputeShader::s_nullSRVs[8]  = { NULL };
		ID3D11ShaderResourceView*  const ComputeShader::s_nullSRV[1]   = { NULL };

		void ComputeShader::Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			//Bind();
			utils::D3D::Instance()->GetDeviceContext()->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}

		void ComputeShader::DispatchIndirect(ID3D11Buffer* indirectBuffer, unsigned alignedByteOffsetForArgs)
		{
			utils::D3D::Instance()->GetDeviceContext()->DispatchIndirect(indirectBuffer, alignedByteOffsetForArgs);
		}

		void ComputeShader::UnbindAllUAVs()
		{
			utils::D3D::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 8, s_nullUAVs, nullptr);
		}

		void ComputeShader::UnbindOneUAV(unsigned startIndex)
		{
			if (startIndex > 7)
			{
				LOG("start index is to large. early return");
				return;
			}
			utils::D3D::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(startIndex, 1, s_nullUAV, nullptr);
		}

		void ComputeShader::UnbindAllSRVs()
		{
			utils::D3D::Instance()->GetDeviceContext()->CSSetShaderResources(0, 8, s_nullSRVs);
		}

		void ComputeShader::UnbindOneSRV(unsigned startIndex)
		{
			if (startIndex > 7)
			{
				LOG("start index is to large. early return");
				return;
			}
			utils::D3D::Instance()->GetDeviceContext()->CSSetShaderResources(startIndex, 1, s_nullSRV);
		}

	}
}