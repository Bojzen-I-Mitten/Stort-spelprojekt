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
			void RenderSkinnedComponent::SubmitPart(Camera* camera, unsigned int i)
			{
				resource::Material* material = m_materials.size() > i ? m_materials[i] : nullptr;
				if (material == nullptr)
					material = resource::Material::GetStandardMaterial();
				std::shared_ptr<graphics::Mesh> mesh = m_model->GetMeshes()[i];

				thomas::graphics::Renderer::SubmitCommand(thomas::graphics::RenderCommand(m_gameObject->m_transform->GetWorldMatrix(), mesh.get(), material, camera));
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
				else
					m_skeleton = std::make_unique<graphics::animation::AnimatedSkeleton>(*model->GetSkeleton());
			}
		}
	}
}
