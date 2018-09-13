#include "ShaderProperty.h"


namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{

			ShaderProperty::ShaderProperty(Type type)
				: m_type(type)
			{}
			void ShaderProperty::Apply(Shader* shader) const
			{
				Apply(m_name, shader);
			}
			ShaderProperty::Type ShaderProperty::GetType() const
			{
				return m_type;
			}
			std::string ShaderProperty::GetName() const 
			{
				return m_name;
			}
			void ShaderProperty::SetName(std::string name) 
			{
				m_name = name;
			}

		}
	}
}
