#include "ShaderPropertyMatrix.h"


namespace thomas
{
	namespace resource
	{
		namespace shaderProperty
		{
#pragma region MatrixProperty

			ShaderPropertyMatrix::ShaderPropertyMatrix(math::Matrix value) 
				: ShaderProperty(Type::MATRIX), m_value(value) 
			{}
			void ShaderPropertyMatrix::Apply(std::string name, Shader* shader)
			{

				shader->GetEffect()->GetVariableByName(name.c_str())->AsMatrix()->SetMatrix(*m_value.m);
			}
			ShaderProperty* ShaderPropertyMatrix::GetDefault() { return new ShaderPropertyMatrix(math::Matrix()); }
			math::Matrix ShaderPropertyMatrix::GetValue() { return m_value; }

#pragma endregion


#pragma region MatrixArrayProperty

			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray()
				: ShaderProperty(Type::MATRIX_ARRAY), m_value(), m_offset(0), m_num_matrix(0)
			{			
			}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int num_matrix)
				: ShaderProperty(Type::MATRIX_ARRAY), m_value(new math::Matrix[num_matrix]), m_offset(0), m_num_matrix(num_matrix)
			{
				std::memcpy(m_value.get(), value, num_matrix * sizeof(math::Matrix));
			}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int offset, unsigned int num_matrix)
				: ShaderProperty(Type::MATRIX_ARRAY), m_value(new math::Matrix[num_matrix]), m_offset(offset), m_num_matrix(num_matrix)
			{
				std::memcpy(m_value.get(), value, num_matrix * sizeof(math::Matrix));
			}
			void ShaderPropertyMatrixArray::Apply(std::string name, Shader* shader) {

				shader->GetEffect()->GetVariableByName(name.c_str())->AsMatrix()->SetMatrixArray(reinterpret_cast<const float*>(m_value.get()[0][0]), m_offset, m_num_matrix);
			}
			ShaderProperty* ShaderPropertyMatrixArray::GetDefault()
			{
				return new ShaderPropertyMatrixArray();
			}
			math::Matrix* ShaderPropertyMatrixArray::GetValue()
			{
				return m_value.get();
			}

#pragma endregion


		}
	}
}