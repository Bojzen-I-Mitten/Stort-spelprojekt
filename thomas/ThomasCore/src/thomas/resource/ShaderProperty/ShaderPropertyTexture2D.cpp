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
					auto test = shader->GetEffect()->GetVariableByName(name.c_str());
					HRESULT hr = test->AsShaderResource()->SetResource(m_value->GetResourceView());
					int stop = 0;
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
			ShaderPropertyTexture2DArray::ShaderPropertyTexture2DArray(Texture2DArray* value) : ShaderProperty(Type::TEXTURE2DARRAY), m_value(value)
			{
				
			};

			void ShaderPropertyTexture2DArray::Apply(std::string name, Shader* shader) const
			{
				
				shader->GetEffect()->GetVariableByName(name.c_str())->AsShaderResource()->SetResource(m_value->GetResourceView());
				
			}

			ShaderProperty* ShaderPropertyTexture2DArray::GetDefault()
			{
				return new ShaderPropertyTexture2DArray(nullptr);
			}

			Texture2DArray* ShaderPropertyTexture2DArray::GetValue()
			{
				return m_value;
			}
#pragma endregion


#pragma region TextureCube
			ShaderPropertyTextureCube::ShaderPropertyTextureCube(TextureCube* value) : ShaderProperty(Type::TEXTURECUBE), m_value(value)
			{

			};

			void ShaderPropertyTextureCube::Apply(std::string name, Shader* shader) const
			{

				shader->GetEffect()->GetVariableByName(name.c_str())->AsShaderResource()->SetResource(m_value->GetResourceView());

			}

			ShaderProperty* ShaderPropertyTextureCube::GetDefault()
			{
				return new ShaderPropertyTextureCube(nullptr);
			}

			TextureCube* ShaderPropertyTextureCube::GetValue()
			{
				return m_value;
			}
#pragma endregion
		}
	}
}