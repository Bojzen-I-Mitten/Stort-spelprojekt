#include "RenderSkinnedComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../ThomasTime.h"
#include "../../resource/Model.h"
#include "../../graphics/Renderer.h"
#include "../../resource/Material.h"
#include "../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{


			RenderSkinnedComponent::RenderSkinnedComponent()
				: m_skeleton() 
			{
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
					LOG("Warning! Attempt to set model with no skinning information in RenderSkinnedComponent.");
				}
				else{
					m_skeleton = std::make_unique<graphics::animation::AnimatedSkeleton>(*model->GetSkeleton());
					insertProperty(m_skeleton->getShaderProperty());
				}
			}
		}
	}
}
