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

			ComputeShader();
			~ComputeShader();
		public:
			void Dispatch(int threadGroupX, int threadGroupY = 1, int threadGroupZ = 1);
			
		};
	}
}