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
			bool ShaderProperty::equals(const ShaderProperty & other) const
			{
				return m_type == other.m_type && m_name == other.m_name;
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
