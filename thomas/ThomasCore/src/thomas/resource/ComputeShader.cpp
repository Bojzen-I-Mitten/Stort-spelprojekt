#include "ComputeShader.h"
#include "ShaderProperty\shaderProperties.h"
#include "../ThomasCore.h"

namespace thomas
{
	namespace resource
	{
		ID3D11UnorderedAccessView* const ComputeShader::s_nullUAVs[8]  = { NULL };
		ID3D11UnorderedAccessView* const ComputeShader::s_nullUAV[1]   = { NULL };
		ID3D11ShaderResourceView*  const ComputeShader::s_nullSRVs[8]  = { NULL };
		ID3D11ShaderResourceView*  const ComputeShader::s_nullSRV[1]   = { NULL };

		ComputeShader::ComputeShader(ID3DX11Effect * effect, std::string path)
			: Shader(effect, path)
		{
		}

		ComputeShader::~ComputeShader()
		{
		}
		std::unique_ptr<resource::ComputeShader> ComputeShader::CreateComputeShader(std::string path)
		{
			ID3DX11Effect* effect = NULL;
			Compile(path, &effect);
			if (!effect)
				return std::unique_ptr<resource::ComputeShader>();

			std::unique_ptr < resource::ComputeShader> shader(new ComputeShader(effect, path));
			if (shader->hasPasses())
				LOG("Compute shader: " << path << " contains no techniques or passes");
			return std::move(shader);
		}

		void ComputeShader::Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ)
		{
			//Bind();
			utils::D3D::Instance()->GetDeviceContextDeffered()->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
		}

		void ComputeShader::DispatchIndirect(ID3D11Buffer* indirectBuffer, unsigned alignedByteOffsetForArgs)
		{
			utils::D3D::Instance()->GetDeviceContextDeffered()->DispatchIndirect(indirectBuffer, alignedByteOffsetForArgs);
		}

		void ComputeShader::UnbindAllUAVs()
		{
			utils::D3D::Instance()->GetDeviceContextDeffered()->CSSetUnorderedAccessViews(0, 8, s_nullUAVs, nullptr);
		}

		void ComputeShader::UnbindOneUAV(unsigned startIndex)
		{
			if (startIndex > 7)
			{
				LOG("start index is to large. early return");
				return;
			}
			utils::D3D::Instance()->GetDeviceContextDeffered()->CSSetUnorderedAccessViews(startIndex, 1, s_nullUAV, nullptr);
		}

		void ComputeShader::UnbindAllSRVs()
		{
			utils::D3D::Instance()->GetDeviceContextDeffered()->CSSetShaderResources(0, 8, s_nullSRVs);
		}

		void ComputeShader::UnbindOneSRV(unsigned startIndex)
		{
			if (startIndex > 7)
			{
				LOG("start index is to large. early return");
				return;
			}
			utils::D3D::Instance()->GetDeviceContextDeffered()->CSSetShaderResources(startIndex, 1, s_nullSRV);
		}

	}
}