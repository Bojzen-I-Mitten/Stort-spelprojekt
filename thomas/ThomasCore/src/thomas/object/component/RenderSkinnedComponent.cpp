#include "RenderSkinnedComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../ThomasTime.h"
#include "../../graphics/Mesh.h"
#include "../../resource/Material.h"
#include "../../resource/Model.h"
#include "../../graphics/Renderer.h"
#include "../../graphics/RenderConstants.h"
#include "../../resource/Material.h"
#include "../GameObject.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../resource/Shader.h"
#include "../../ThomasCore.h"
#include "../../utils/AutoProfile.h"
#include "../../resource/ComputeShader.h"

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
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->GetTransform()->GetWorldMatrix());
#endif
				EDITOR_LOCK();
				if (m_skeleton)
				{
					PROFILE("AnimationUpdate")
					// Updates skin matrices
					m_skeleton->update(ThomasTime::GetDeltaTime());
				}
			}

			void RenderSkinnedComponent::SubmitPart(Camera * camera, unsigned int i, const thomas::resource::shaderproperty::ShaderPropertyStatic * property_data, uint32_t num_prop)
			{

				resource::Material* material = m_materials.size() > i ? m_materials[i] : nullptr;
				if (material == nullptr)
					material = resource::Material::GetStandardMaterial();

				std::shared_ptr<graphics::Mesh> mesh = m_model->GetMeshes()[i];

				resource::ComputeShader* skinningCS = graphics::Renderer::Instance()->GetSkinningShader();


				skinningCS->Bind();
				skinningCS->SetPass(0);

				property_data[0].m_apply(property_data[0], skinningCS);
				mesh->Skin(skinningCS);
				
				RenderComponent::SubmitPart(camera, i, property_data, num_prop);
			}

			void RenderSkinnedComponent::SetMaterial(int meshIndex, resource::Material* material) {
				RenderComponent::SetMaterial(meshIndex, material);
				EDITOR_LOCK();
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
				
				EDITOR_LOCK();
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
