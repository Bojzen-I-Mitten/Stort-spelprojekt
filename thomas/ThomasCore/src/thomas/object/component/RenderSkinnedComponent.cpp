#include "RenderSkinnedComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../ThomasTime.h"
#include "../../resource/Model.h"
#include "../../graphics/Renderer.h"
#include "../../graphics/RenderConstants.h"
#include "../../resource/Material.h"
#include "../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{


			RenderSkinnedComponent::RenderSkinnedComponent()
				: m_skinArray(0), m_skeleton()
			{
			}
			RenderSkinnedComponent::~RenderSkinnedComponent() {

			}
			void RenderSkinnedComponent::Update()
			{
				RenderComponent::Update();
				if (m_skeleton) {
					m_skeleton->update(ThomasTime::GetDeltaTime());
				}
			}

			void RenderSkinnedComponent::SetMaterial(int meshIndex, resource::Material* material) {
				RenderComponent::SetMaterial(meshIndex, material);
				if (material->GetShader()->GetPropertyIndex(graphics::THOMAS_MATRIX_SKIN_ARRAY_HASH, m_skinInfo.m_effect_id))
					LOG("Warning! Material applied to skinned render component does not use any bone information.");
				applySkin();
				m_skinInfo.m_apply = resource::shaderproperty::ApplyEffectMatrixDynamic;
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
					insertProperty(&m_skinArray);
					return true;	// Return true only if model is applied correctly
				}
				return false;
			}
			graphics::animation::IBlendTree* RenderSkinnedComponent::GetBlendTree() {
				return m_skeleton.get();
			}
			void RenderSkinnedComponent::applySkin()
			{
				if (m_skeleton) {
					m_skinInfo.m_dataSize = m_skeleton->boneCount() * 64;
					m_skinInfo.m_data = m_skeleton->;
				}
				else {
					m_skinInfo.m_dataSize = m_skeleton->boneCount() * 64;
					m_skinInfo.m_data = NULL;
				}
			}
		}
	}
}
