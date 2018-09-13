#pragma once
#include "ShaderProperty.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderProperty
		{
			class ShaderPropertyMatrix : public ShaderProperty
			{
			public:

				ShaderPropertyMatrix(math::Matrix value);
				void Apply(std::string name, Shader* shader);
				static ShaderProperty* GetDefault();
				math::Matrix GetValue();
			private:
				math::Matrix m_value;
			};
		}
	}
}