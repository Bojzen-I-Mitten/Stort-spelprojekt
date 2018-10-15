#pragma once
#include "ShaderProperty.h"
#include "../texture/Texture2D.h"
#include "../texture/Texture2DArray.h"
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
			
			class ShaderPropertyTexture2DArray : public ShaderProperty
			{
			public:
				ShaderPropertyTexture2DArray(Texture2DArray* value);
				void Apply(std::string name, Shader* shader) const;
				static ShaderProperty* GetDefault();
				Texture2DArray* GetValue();

			private:
				Texture2DArray* m_value;
				unsigned m_nrOfTextures;
			};

		}
	}
}