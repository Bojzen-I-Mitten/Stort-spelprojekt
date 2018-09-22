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
			ShaderProperty::ShaderProperty(Type type, const std::string &name)
				: m_name(name), m_type(type)
			{
			}
			void ShaderProperty::Apply(Shader* shader) const
			{
				Apply(m_name, shader);
			}
			bool ShaderProperty::equals(const ShaderProperty & other) const
			{
				return m_type == other.m_type && m_name == other.m_name;
			}
			ShaderProperty * ShaderProperty::copy() const
			{
				return nullptr;
			}
			ShaderProperty::Type ShaderProperty::GetType() const
			{
				return m_type;
			}
			const std::string& ShaderProperty::GetName() const 
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
