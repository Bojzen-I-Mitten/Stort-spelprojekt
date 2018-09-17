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
				m_skinArray.SetName(graphics::THOMAS_MATRIX_SKIN_ARRAY);
			}
			RenderSkinnedComponent::~RenderSkinnedComponent() {

			}
			void RenderSkinnedComponent::Update()
			{
				RenderComponent::Update();
				if (m_skeleton) {
					m_skeleton->update(ThomasTime::GetDeltaTime());

					math::Matrix world = m_gameObject->m_transform->GetWorldMatrix();

					math::Vector4 v0 = math::Vector4(-0.5f, 0.5f, 0.f, 1);
					math::Vector4 v1 = math::Vector4(0.f, -0.5f, 0.f, 1);
					math::Vector4 v2 = math::Vector4(0.5f, 0.5f, 0.f, 1);
					//math::Vector4 s = math::Vector4::Transform(v, _ref.getBone(0)._invBindPose);
					//v = math::Vector4::Transform(v, m_skinArray.GetValue()[0]);
					v0 = math::Vector4::Transform(v0, m_skinArray.GetValue()[0]);
					v1 = math::Vector4::Transform(v1, m_skinArray.GetValue()[0]);
					v2 = math::Vector4::Transform(v2, m_skinArray.GetValue()[0]);
					world = m_skinArray.GetValue()[0] * world;
					v0 = math::Vector4::Transform(v0, world);
					v1 = math::Vector4::Transform(v1, world);
					v2 = math::Vector4::Transform(v2, world);
				}
			}
			void RenderSkinnedComponent::SetModel(resource::Model * model)
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
						new graphics::animation::AnimatedSkeleton(*model->GetSkeleton(), m_skinArray));
					insertProperty(&m_skinArray);
				}
			}
		}
	}
}
