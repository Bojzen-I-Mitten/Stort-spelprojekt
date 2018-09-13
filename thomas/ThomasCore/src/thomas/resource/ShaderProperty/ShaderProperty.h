#pragma once
#include <string>
#include "../Shader.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderProperty
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

				virtual void Apply(std::string name, Shader* shader) = 0;
				void Apply(Shader* shader);

			public:
				Type GetType();
				std::string GetName();
				void SetName(std::string name);
			private:
				std::string m_name;
				Type m_type;
			};
		}
	}
}