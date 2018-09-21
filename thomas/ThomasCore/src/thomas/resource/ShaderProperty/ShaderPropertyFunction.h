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
			// Typedeffed shader property function
			typedef void (*ShaderPropertyFunction)(ShaderPropertyStruct& info, Shader *s);

			struct ShaderPropertyStruct {
				uint32_t m_effect_id;						// Static: Effect index. Dynamic: Effect name hash
				ShaderPropertyFunction m_apply;				// Effect apply function
				void * m_data;								// Storage data info
				uint32_t m_dataSize;						// Data size in bytes

#ifdef _DEBUG
				ShaderProperty::Type m_type;				// Effect types
				std::string m_effectName;					// Effect name for debug info
#endif
			};

			void ApplyEffectMatrix(ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixDynamic(ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixArray(ShaderPropertyStruct& info, Shader* shader);
			void ApplyEffectMatrixDynamicArray(ShaderPropertyStruct& info, Shader* shader);


		}
	}
}