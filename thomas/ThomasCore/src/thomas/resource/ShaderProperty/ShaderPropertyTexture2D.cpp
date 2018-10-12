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
			ShaderPropertyTexture2DArray::ShaderPropertyTexture2DArray(Texture2D** value, unsigned nrOfTextures) : ShaderProperty(Type::TEXTURE2DARRAY), m_value(value)
			{
				for (unsigned i = 0; i < nrOfTextures; ++i)
				{
					m_srvs.push_back(value[i]->GetResourceView());
				}
				
			};

			void ShaderPropertyTexture2DArray::Apply(std::string name, Shader* shader) const
			{
				if (m_srvs.size() > 0)
				{
					shader->GetEffect()->GetVariableByName(name.c_str())->AsShaderResource()->SetResourceArray((ID3D11ShaderResourceView**)m_srvs.data(), 0, m_srvs.size());
				}
			}

			ShaderProperty* ShaderPropertyTexture2DArray::GetDefault()
			{
				return new ShaderPropertyTexture2DArray(nullptr, 0);
			}

			Texture2D** ShaderPropertyTexture2DArray::GetValue()
			{
				return m_value;
			}
#pragma endregion
		}
	}
}