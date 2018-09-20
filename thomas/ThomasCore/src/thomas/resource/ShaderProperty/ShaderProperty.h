#pragma once
#include <string>
#include "../Shader.h"
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
					SCALAR_FLOAT,
					SCALAR_BOOL,
					SCALAR_INT,
					VECTOR,
					COLOR,
					MATRIX,
					MATRIX_ARRAY,
					SHADER_RESOURCE,
					CONSTANT_BUFFER,
					TEXTURE2D
				};

				ShaderProperty(Type type);
				virtual ~ShaderProperty() {}

				virtual void Apply(std::string name, Shader* shader) const = 0;
				void Apply(Shader* shader) const;
				bool equals(const ShaderProperty &other) const;

			public:
				Type GetType() const;
				std::string GetName() const;
				void SetName(std::string name);

			private:
				std::string m_name;
				Type m_type;
			};
		}
	}
}