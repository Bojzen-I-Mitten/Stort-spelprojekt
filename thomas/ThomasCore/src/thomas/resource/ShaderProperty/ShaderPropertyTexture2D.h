#pragma once
#include "ShaderProperty.h"
#include "../texture/Texture2D.h"
#include "../Shader.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderproperty
		{
			class ShaderPropertyTexture2D : public ShaderProperty
			{
			public:

				ShaderPropertyTexture2D(Texture2D* value) : ShaderProperty(Type::TEXTURE2D), m_value(value) {};
				void Apply(std::string name, Shader* shader) const
				{
					if (m_value)
					{
						shader->GetEffect()->GetVariableByName(name.c_str())->AsShaderResource()->SetResource(m_value->GetResourceView());
					}
				}
				static ShaderProperty* GetDefault() { return new ShaderPropertyTexture2D(Texture2D::GetWhiteTexture()); }
				Texture2D* GetValue() { return m_value; }
			private:
				Texture2D* m_value;
			};
		}
	}
}