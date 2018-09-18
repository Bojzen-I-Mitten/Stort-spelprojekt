#pragma once
#include "ShaderProperty.h"
#include "../../utils/d3d.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderProperty
		{
			class ShaderPropertyUnorderedAccessView : public ShaderProperty
			{
			public:

				ShaderPropertyUnorderedAccessView(ID3D11UnorderedAccessView* value) : ShaderProperty(Type::UNORDERED_ACCESS), m_value(value) {};
				void Apply(std::string name, Shader* shader)
				{
					shader->GetEffect()->GetVariableByName(name.c_str())->AsUnorderedAccessView()->SetUnorderedAccessView(m_value);
				}
				static ShaderProperty* GetDefault() { return new ShaderPropertyUnorderedAccessView(nullptr); }
				ID3D11UnorderedAccessView* GetValue() { return m_value; }
			private:
				ID3D11UnorderedAccessView* m_value;
			};
		}
	}
}