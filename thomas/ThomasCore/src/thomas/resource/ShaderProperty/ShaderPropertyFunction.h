#pragma once



#include <string>
#include "ShaderProperty.h"
#include <stdarg.h>

namespace thomas
{
	namespace resource
	{
		class Shader;
		namespace shaderproperty
		{
			struct ShaderPropertyStruct; 
			// Typedeffed shader property function
			typedef void (*ShaderPropertyFunction)(const ShaderPropertyStruct& info, Shader *s);

			struct ShaderPropertyStruct {
				uint32_t m_effect_id;						// Static: Effect index. Dynamic: Effect name hash
				ShaderPropertyFunction m_apply;				// Effect apply function
				const void * m_data;						// Storage data info
				uint32_t m_dataSize;						// Data size in bytes

#ifdef _DEBUG
				ShaderProperty::Type m_type;				// Effect types
				std::string m_effectName;					// Effect name for debug info
#endif
				/* Compare the property structs by ID (note dynamic and static variants will compare true)
				*/
				bool operator==(const ShaderPropertyStruct &other) const;
			};

			typedef struct ShaderPropertyStruct ShaderPropertyDynamic;
			typedef struct ShaderPropertyStruct ShaderPropertyStatic;

			void ApplyEffectMatrix(const ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixDynamic(const ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixArray(const ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixDynamicArray(const ShaderPropertyStruct& info, Shader* shader);


		}
	}
}