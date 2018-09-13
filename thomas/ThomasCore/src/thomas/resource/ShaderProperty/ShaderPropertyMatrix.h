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


			class ShaderPropertyMatrixArray : public ShaderProperty
			{
			private:
				ShaderPropertyMatrixArray();
			public:
				ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int num_matrix);
				ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int offset, unsigned int num_matrix);
				void Apply(std::string name, Shader* shader);
				static ShaderProperty* GetDefault();
				math::Matrix* GetValue();
			private:
				std::unique_ptr<math::Matrix> m_value;
				unsigned int m_offset;
				unsigned int m_num_matrix;
			};
		}
	}
}