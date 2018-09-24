#pragma once
#include "Material.h"

namespace thomas
{
	namespace resource
	{
		//rework
		class ComputeShader
		{
		public:
			ComputeShader();
			void Dispatch(int threadGroupX, int threadGroupY, int threadGroupZ);
			void SetUAV(const std::string& name, ID3D11UnorderedAccessView& value);
			ID3D11UnorderedAccessView* GetUAV(const std::string& name);
		};
	}
}