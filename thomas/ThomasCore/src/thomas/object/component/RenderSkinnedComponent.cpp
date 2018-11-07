#include "RenderSkinnedComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../ThomasTime.h"
#include "../../resource/Model.h"
#include "../../graphics/Renderer.h"
#include "../../graphics/RenderConstants.h"
#include "../../resource/Material.h"
#include "../GameObject.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../resource/Shader.h"
#include "../../ThomasCore.h"
#include "../../utils/AutoProfile.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{


			RenderSkinnedComponent::RenderSkinnedComponent()
				: m_skeleton()
			{
				resource::shaderproperty::ShaderPropertyStatic prop;
				prop.m_apply = resource::shaderproperty::ApplyEffectMatrixDynamicArray;
				prop.m_dataSize = 0;
				prop.m_data = NULL;
				prop.m_effect_id = graphics::THOMAS_MATRIX_SKIN_ARRAY_HASH;
#ifdef _DEBUG
				prop.m_effectName = graphics::THOMAS_MATRIX_SKIN_ARRAY;
				prop.m_type = resource::shaderproperty::ShaderPropertyType::MATRIX_ARRAY;
#endif
				m_skinInfo = &insertProperty(prop);
			}
			RenderSkinnedComponent::~RenderSkinnedComponent() {

			}
			void RenderSkinnedComponent::Update()
			{
				RenderComponent::Update();
#ifdef _EDITOR
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->m_transform->GetWorldMatrix());
#endif
				if (m_skeleton)
				{
					PROFILE("AnimationUpdate")
					m_skeleton->update(ThomasTime::GetDeltaTime());
				}
			}

			void RenderSkinnedComponent::SetMaterial(int meshIndex, resource::Material* material) {
				RenderComponent::SetMaterial(meshIndex, material);
				uint32_t effectIndex;
				if (!material || material->GetShader()->GetPropertyIndex(graphics::THOMAS_MATRIX_SKIN_ARRAY_HASH, effectIndex)) {
					m_skinInfo->m_apply = thomas::resource::shaderproperty::ApplyEffectMatrixDynamicArray;
					m_skinInfo->m_effect_id = graphics::THOMAS_MATRIX_SKIN_ARRAY_HASH;
					LOG("Warning! Material applied to skinned render component does not use any bone information.");
				}
				else {
					m_skinInfo->m_apply = thomas::resource::shaderproperty::ApplyEffectMatrixArray;
					m_skinInfo->m_effect_id = effectIndex;
				}
			}
			bool RenderSkinnedComponent::SetModel(resource::Model * model)
			{
				RenderComponent::SetModel(model);
				// Read new skeleton
				if (!model)
					m_skeleton = NULL;
				else if (model->GetSkeleton() == NULL) {
					LOG("Warning! Attempt to set model with no skinning information in RenderSkinnedComponent.");
				}
				else{
					m_skeleton = std::unique_ptr<graphics::animation::AnimatedSkeleton>(
						new graphics::animation::AnimatedSkeleton(*model->GetSkeleton()));
					applySkin();
					return true;	// Return true only if model is applied correctly
				}
				return false;
			}
			graphics::animation::IBlendTree* RenderSkinnedComponent::GetBlendTree() {
				if (m_skeleton)
					return m_skeleton.get();
				else
					return nullptr;
			}
			void RenderSkinnedComponent::applySkin()
			{
				if (m_skeleton) {
					m_skinInfo->m_dataSize = m_skeleton->boneCount() * sizeof(math::Matrix);
					m_skinInfo->m_data = m_skeleton->getSkin();
				}
				else {
					m_skinInfo->m_dataSize = 0;
					m_skinInfo->m_data = NULL;
				}
			}
		}
	}
}
