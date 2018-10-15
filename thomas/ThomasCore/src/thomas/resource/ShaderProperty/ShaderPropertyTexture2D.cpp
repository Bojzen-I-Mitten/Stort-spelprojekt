#include "ShaderPropertyTexture2D.h"

namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{

#pragma region Texture2DProperty

			ShaderPropertyTexture2D::ShaderPropertyTexture2D(Texture2D* value) : ShaderProperty(Type::TEXTURE2D), m_value(value) 
			{
			
			};

			void ShaderPropertyTexture2D::Apply(std::string name, Shader* shader) const
			{
				if (m_value)
				{
					shader->GetEffect()->GetVariableByName(name.c_str())->AsShaderResource()->SetResource(m_value->GetResourceView());
				}
			}

			ShaderProperty* ShaderPropertyTexture2D::GetDefault() 
			{ 
				return new ShaderPropertyTexture2D(Texture2D::GetWhiteTexture()); 
			}

			Texture2D* ShaderPropertyTexture2D::GetValue() 
			{ 
				return m_value; 
			}


#pragma endregion

#pragma region Texture2DArrayProperty

#pragma endregion
		}
	}
}