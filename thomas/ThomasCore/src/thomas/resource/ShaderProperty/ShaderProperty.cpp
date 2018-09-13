#include "ShaderProperty.h"


namespace thomas
{
	namespace resource
	{
		namespace shaderProperty
		{

			ShaderProperty::ShaderProperty(Type type)
				: m_type(type)
			{}
			void ShaderProperty::Apply(Shader* shader) {
				Apply(m_name, shader);
			}
			ShaderProperty::Type ShaderProperty::GetType()
			{
				return m_type;
			}
			std::string ShaderProperty::GetName() {
				return m_name;
			}
			void ShaderProperty::SetName(std::string name) {
				m_name = name;
			}

		}
	}
}
