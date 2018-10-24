#pragma once



namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{
			enum class ShaderPropertyType
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
				TEXTURE2D = 9
			};
		}
	}
}