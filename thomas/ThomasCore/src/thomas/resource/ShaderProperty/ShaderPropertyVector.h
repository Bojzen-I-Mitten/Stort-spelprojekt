#pragma once
#include "ShaderProperty.h"
#include "../../utils/Math.h"
#include "../Shader.h"
namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{
			class ShaderPropertyVector : public ShaderProperty
			{
			public:

				ShaderPropertyVector(math::Vector4 value) : ShaderProperty(Type::VECTOR), m_value(value) {};
				void Apply(std::string name, Shader* shader) const
				{
					HRESULT result = shader->GetEffect()->GetVariableByName(name.c_str())->AsVector()->SetFloatVector((float*)&m_value);
				}
				static ShaderProperty* GetDefault() { return new ShaderPropertyVector(math::Vector4()); }
				math::Vector4 GetValue() { return m_value; }
			private:
				math::Vector4 m_value;
			};
		}
	}
}