#include "ShaderPropertyMatrix.h"
#include "../Shader.h"


namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{
#pragma region MatrixProperty

			ShaderPropertyMatrix::ShaderPropertyMatrix(math::Matrix value) 
				: ShaderProperty(Type::MATRIX), m_value(value) 
			{}
			void ShaderPropertyMatrix::Apply(std::string name, Shader* shader) const
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
			ShaderPropertyMatrixArray::~ShaderPropertyMatrixArray()
			{
			}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(unsigned int num_matrix)
				: ShaderProperty(Type::MATRIX_ARRAY), m_value(num_matrix == 0 ? NULL : new math::Matrix[num_matrix]), m_offset(0), m_num_matrix(num_matrix)
			{
			}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int num_matrix)
				: ShaderPropertyMatrixArray(value, 0, num_matrix)
			{	}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(const math::Matrix * value, unsigned int offset, unsigned int num_matrix)
				: ShaderProperty(Type::MATRIX_ARRAY), m_value(new math::Matrix[num_matrix]), m_offset(offset), m_num_matrix(num_matrix)
			{
				std::memcpy(m_value.get(), value, num_matrix * sizeof(math::Matrix));
			}
			ShaderPropertyMatrixArray::ShaderPropertyMatrixArray(const std::string &name, const math::Matrix * value, unsigned int offset, unsigned int num_matrix)
				: ShaderProperty(Type::MATRIX_ARRAY, name), m_value(new math::Matrix[num_matrix]), m_offset(offset), m_num_matrix(num_matrix)
			{
				std::memcpy(m_value.get(), value, num_matrix * sizeof(math::Matrix));
			}
			void ShaderPropertyMatrixArray::Apply(std::string name, Shader* shader) const
			{
				shader->GetEffect()->GetVariableByName(name.c_str())->AsMatrix()->SetMatrixArray(*m_value.get()->m, m_offset, m_num_matrix);
			}
			ShaderProperty* ShaderPropertyMatrixArray::GetDefault()
			{
				return new ShaderPropertyMatrixArray();
			}
			math::Matrix* ShaderPropertyMatrixArray::GetValue()
			{
				return m_value.get();
			}

			void ShaderPropertyMatrixArray::resize(unsigned int num_matrix)
			{
				m_value = std::unique_ptr<math::Matrix>(new math::Matrix[num_matrix]);
				m_offset = 0;
				m_num_matrix = num_matrix;
			}

			ShaderProperty * ShaderPropertyMatrixArray::copy() const
			{
				return new ShaderPropertyMatrixArray(GetName(), m_value.get(), m_offset, m_num_matrix);
			}

			math::Matrix& ShaderPropertyMatrixArray::Matrix(unsigned int index)
			{
				assert(index < m_num_matrix);
				return m_value.get()[index];
			}
#pragma endregion


		}
	}
}