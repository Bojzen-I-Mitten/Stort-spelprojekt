#include "ShaderPropertyStruct.h"
#include "../Shader.h"

namespace thomas
{
	namespace resource
	{
		namespace shaderproperty
		{
			bool ShaderPropertyStruct::operator==(const ShaderPropertyStruct& other) const
			{
#ifdef _DEBUG
				if (m_effect_id == other.m_effect_id) {
					assert(m_type == m_type && m_effectName == other.m_effectName);	// Verify everything is correct.
					return true;
				}
				return false;
#else
				return  m_effect_id == other.m_effect_id;
#endif
			}
			void ApplyEffectMatrix(const ShaderPropertyStruct& info, Shader* shader)
			{
				shader->GetEffect()->GetVariableByIndex(info.m_effect_id)->AsMatrix()->SetMatrixTranspose(reinterpret_cast<const float*>(info.m_data));
			}
			void ApplyEffectMatrixDynamic(const ShaderPropertyStruct& info, Shader* shader)
			{
				uint32_t index;
				if (shader->GetPropertyIndex(info.m_effect_id, index)) 
					return;
				shader->GetEffect()->GetVariableByIndex(index)->AsMatrix()->SetMatrixTranspose(reinterpret_cast<const float*>(info.m_data));
			}


			void ApplyEffectMatrixArray(const ShaderPropertyStruct& info, Shader* shader)
			{
				if (info.m_dataSize == 0)
					return;
				shader->GetEffect()->GetVariableByIndex(info.m_effect_id)->AsMatrix()->SetMatrixTransposeArray(reinterpret_cast<const float*>(info.m_data), 0, info.m_dataSize / 64);
			}
			void ApplyEffectMatrixDynamicArray(const ShaderPropertyStruct& info, Shader* shader)
			{
				uint32_t index;
				if (shader->GetPropertyIndex(info.m_effect_id, index) || info.m_dataSize == 0)
					return;
				shader->GetEffect()->GetVariableByIndex(index)->AsMatrix()->SetMatrixTransposeArray(reinterpret_cast<const float*>(info.m_data), 0, info.m_dataSize / 64);
			}

		}
	}
}