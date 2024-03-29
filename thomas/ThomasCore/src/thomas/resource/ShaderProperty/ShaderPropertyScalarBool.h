#pragma once
#include "ShaderProperty.h"
#include "../Shader.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderproperty
		{
			class ShaderPropertyScalarBool : public ShaderProperty
			{
			public:

				ShaderPropertyScalarBool(bool value) : ShaderProperty(Type::SCALAR_BOOL), m_value(value) {};
				void Apply(std::string name, Shader* shader) const
				{
					shader->GetEffect()->GetVariableByName(name.c_str())->AsScalar()->SetBool(m_value);
				}
				static ShaderProperty* GetDefault() { return new ShaderPropertyScalarBool(0); }
				bool GetValue() { return m_value; }
			private:
				bool m_value;
			};
		}
	}
}