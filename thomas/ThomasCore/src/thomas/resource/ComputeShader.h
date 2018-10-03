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
			
		public:
			void Dispatch(int threadGroupX, int threadGroupY = 1, int threadGroupZ = 1);
			void DispatchIndirect(ID3D11Buffer* indirectBuffer, unsigned alignedByteOffsetForArgs = 0);
		};
	}
}