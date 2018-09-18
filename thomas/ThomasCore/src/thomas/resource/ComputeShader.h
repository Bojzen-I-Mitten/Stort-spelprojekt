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

			ComputeShader(ID3DX11Effect* effect, std::string path);
		public:
			void Dispatch(int threadGroupX, int threadGroupY = 0, int threadGroupZ = 0);
			static ComputeShader* CreateComputeShader();
		};
	}
}