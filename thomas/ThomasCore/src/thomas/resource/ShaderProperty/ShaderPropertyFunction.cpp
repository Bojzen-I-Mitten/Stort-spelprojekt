#include "ShaderPropertyFunction.h"

namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{
			void ApplyEffectMatrix(ShaderPropertyStruct& info, Shader* shader)
			{
				shader->GetEffect()->GetVariableByIndex(info.m_effect_id)->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(info.m_data));
			}
			void ApplyEffectMatrixDynamic(ShaderPropertyStruct& info, Shader* shader)
			{
				uint32_t index;
				if (shader->GetPropertyIndex(info.m_effect_id, index)) 
					return;
				shader->GetEffect()->GetVariableByIndex(index)->AsMatrix()->SetMatrix(reinterpret_cast<const float*>(info.m_data));
			}


			void ApplyEffectMatrixArray(ShaderPropertyStruct& info, Shader* shader)
			{
				if (info.m_dataSize == 0)
					return;
				shader->GetEffect()->GetVariableByIndex(info.m_effect_id)->AsMatrix()->SetMatrixArray(reinterpret_cast<const float*>(info.m_data), 0, info.m_dataSize / 64);
			}
			void ApplyEffectMatrixDynamicArray(ShaderPropertyStruct& info, Shader* shader)
			{
				uint32_t index;
				if (shader->GetPropertyIndex(info.m_effect_id, index) || info.m_dataSize == 0)
					return;
				shader->GetEffect()->GetVariableByIndex(index)->AsMatrix()->SetMatrixArray(reinterpret_cast<const float*>(info.m_data), 0, info.m_dataSize / 64);
			}

		}
	}
}