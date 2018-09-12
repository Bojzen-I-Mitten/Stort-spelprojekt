#include "RenderSkinnedComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../ThomasTime.h"
#include "../../resource/Model.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			RenderSkinnedComponent::RenderSkinnedComponent()
				: m_skeleton() {

			}
			RenderSkinnedComponent::~RenderSkinnedComponent() {

			}
			void RenderSkinnedComponent::Update()
			{
				RenderComponent::Update();
				if (m_skeleton)
					m_skeleton->update(ThomasTime::GetDeltaTime());
			}
			void RenderSkinnedComponent::SetModel(resource::Model * model)
			{
				RenderComponent::SetModel(model);
				// Read new skeleton
				if (!model)
					m_skeleton = NULL;
				else if (model->GetSkeleton() == NULL) {
					LOG("Warning! Model set to RenderSkinnedComponent has no skinning information.");
				}
				else
					m_skeleton = std::make_unique<graphics::animation::AnimatedSkeleton>(model->GetSkeleton());
			}
		}
	}
}
