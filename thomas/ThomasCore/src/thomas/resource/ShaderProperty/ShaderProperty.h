#pragma once
#include <string>
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderproperty
		{
			class ShaderProperty
			{
			public:
				enum class Type
				{
					SCALAR_FLOAT = 0,
					SCALAR_BOOL = 1,
					SCALAR_INT = 2,
					VECTOR = 3,
					COLOR = 4,
					MATRIX = 5,
					MATRIX_ARRAY = 6,
					SHADER_RESOURCE = 7,
					CONSTANT_BUFFER = 8,
					TEXTURE2D = 9,
					TEXTURE2DARRAY = 10,
					UNORDERED_ACCESS = 11
				};

				ShaderProperty(Type type);
				ShaderProperty(Type type, const std::string & name);
				virtual ~ShaderProperty() {}

				virtual void Apply(std::string name, Shader* shader) const = 0;
				void Apply(Shader* shader) const;
				bool equals(const ShaderProperty &other) const;

				virtual ShaderProperty* copy() const;

			public:
				Type GetType() const;
				const std::string& GetName() const;
				void SetName(std::string name);

			private:
				std::string m_name;
				Type m_type;
			};
		}
	}
}