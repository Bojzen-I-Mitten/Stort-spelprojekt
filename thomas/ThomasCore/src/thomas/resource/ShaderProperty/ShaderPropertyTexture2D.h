#pragma once
#include "ShaderProperty.h"
#include "../texture/Texture2D.h"
namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderproperty
		{
			class ShaderPropertyTexture2D : public ShaderProperty
			{
			public:
				ShaderPropertyTexture2D(Texture2D* value);
				void Apply(std::string name, Shader* shader) const;
				static ShaderProperty* GetDefault(); 
				Texture2D* GetValue();
			private:
				Texture2D* m_value;
			};
			/*
			class ShaderPropertyTexture2DArray : public ShaderProperty
			{
			public:
				ShaderPropertyTexture2D(Texture2D* value);
				void Apply(std::string name, Shader* shader) const;
				static ShaderProperty* GetDefault();
				Texture2D* GetValue();
			private:
				Texture2D* m_value;
			};*/

		}
	}
}