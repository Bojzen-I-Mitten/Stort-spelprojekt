#include "ShaderPropertyMatrix.h"


namespace thomas
{
	namespace resource
	{
		namespace shaderProperty
		{
			ShaderPropertyMatrix::ShaderPropertyMatrix(math::Matrix value) 
				: ShaderProperty(Type::MATRIX), m_value(value) 
			{}
			void ShaderPropertyMatrix::Apply(std::string name, Shader* shader)
			{

				shader->GetEffect()->GetVariableByName(name.c_str())->AsMatrix()->SetMatrix(*m_value.m);
			}
			ShaderProperty* ShaderPropertyMatrix::GetDefault() { return new ShaderPropertyMatrix(math::Matrix()); }
			math::Matrix ShaderPropertyMatrix::GetValue() { return m_value; }
		}
	}
}