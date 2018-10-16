#pragma once
#include "Material.h"
#include "Shader.h"
#include <d3dx11effect.h>
namespace thomas
{
	namespace resource
	{
		class ComputeShader : public Shader
		{
		private:
			
			static ID3D11UnorderedAccessView* const s_nullUAVs[8];
			static ID3D11UnorderedAccessView* const s_nullUAV[1];
			static ID3D11ShaderResourceView* const s_nullSRVs[8];
			static ID3D11ShaderResourceView* const s_nullSRV[1];
		public:
			void Dispatch(int threadGroupX, int threadGroupY = 1, int threadGroupZ = 1);
			void DispatchIndirect(ID3D11Buffer* indirectBuffer, unsigned alignedByteOffsetForArgs = 0);
			static void UnbindAllUAVs();
			static void UnbindOneUAV(unsigned startIndex);
			static void UnbindAllSRVs();
			static void UnbindOneSRV(unsigned startIndex);
		};
	}
}