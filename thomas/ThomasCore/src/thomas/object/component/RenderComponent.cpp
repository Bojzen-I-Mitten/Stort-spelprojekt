#include "RenderComponent.h"
#include "../../graphics/Mesh.h"
#include "../../resource/Material.h"
#include "../../resource/Model.h"
#include "../GameObject.h"
#include "../../graphics/Renderer.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../resource/ShaderProperty/ShaderProperty.h"
#include "../../System.h"
#include "../../graphics/render/Frame.h"

namespace thomas {
	namespace object {
		namespace component {

			std::vector<RenderComponent*> RenderComponent::s_renderComponents;
			RenderComponent::RenderComponent()
			{
				m_model = nullptr;
				m_bounds = math::BoundingOrientedBox();
				m_bounds.Extents.x = 0;
				m_bounds.Extents.y = 0;
				m_bounds.Extents.z = 0;
			}

			bool RenderComponent::SetModel(resource::Model* model)
			{
				if (!model)
				{
					m_model = model;
					m_bounds = math::BoundingOrientedBox();
					m_bounds.Extents.x = 0;
					m_bounds.Extents.y = 0;
					m_bounds.Extents.z = 0;
				}
				else
				{
					m_model = model;
					if (m_model->GetMeshes().size() < m_materials.size())
						m_materials.resize(m_model->GetMeshes().size());

					while (m_model->GetMeshes().size() > m_materials.size())
						m_materials.push_back(resource::Material::GetStandardMaterial());
				}
				return true;
			}

			void RenderComponent::Update()
			{
				if (m_model)
				{
					math::BoundingOrientedBox::CreateFromBoundingBox(m_bounds, m_model->m_bounds);
					m_bounds.Transform(m_bounds, m_gameObject->m_transform->GetWorldMatrix());

					s_renderComponents.push_back(this);
				
				}		
			}
			void RenderComponent::SetMaterial(resource::Material * material)
			{
				for (unsigned int i = 0; i < m_materials.size(); i++)
					SetMaterial(i, material);
			}
			void RenderComponent::SetMaterial(int meshIndex, resource::Material * material)
			{
				
				if (meshIndex >= 0)
				{
					if (!material)
						material = resource::Material::GetStandardMaterial();
					if (meshIndex >= m_materials.size())
					{
						m_materials.push_back(material);
					}
					else m_materials[meshIndex] = material;
				}
					
			}

			resource::Material * RenderComponent::GetMaterial(int meshIndex)
			{
				if (m_materials.size() > meshIndex && meshIndex >= 0)
					return m_materials[meshIndex];
				else
					return resource::Material::GetStandardMaterial();
			}


			void RenderComponent::SubmitToRenderer(Camera* camera)
			{
				if (m_model)
				{
					for (int i = 0; i < m_model->GetMeshes().size(); i++)
						SubmitPart(camera, i);
				}
			}

			void RenderComponent::OnDestroy()
			{
				for (int i = 0; i < s_renderComponents.size(); i++)
				{
					if (s_renderComponents[i] == this)
					{
						s_renderComponents.erase(s_renderComponents.begin() + i);
						return;
					}
				}
			}

			void RenderComponent::ClearList()
			{
				s_renderComponents.clear();
			}

			std::vector<RenderComponent*> RenderComponent::GetAllRenderComponents()
			{
				return s_renderComponents;
			}

			bool verifyPropertyList(const resource::shaderproperty::ShaderProperty ** arr, size_t count) {
				bool correct = true;
				for (size_t i = 0; i < count; i++)
					correct &= arr[i] != nullptr;
				return correct;
			}

			void RenderComponent::SubmitPart(Camera* camera, unsigned int i)
			{
				resource::Material* material = m_materials.size() > i ? m_materials[i] : nullptr;
				if (material == nullptr)
					material = resource::Material::GetStandardMaterial();

				std::shared_ptr<graphics::Mesh> mesh = m_model->GetMeshes()[i];

				assert(verifyPropertyList(m_properties.data(), m_properties.size()));

				thomas::graphics::render::RenderCommand cmd(
					m_gameObject->m_transform->GetWorldMatrix(), 
					mesh.get(), 
					material, 
					camera, 
					m_properties.size(), 
					m_properties.data());

				System::S_RENDERER.SubmitCommand(cmd);
			}

			void RenderComponent::insertProperty(const resource::shaderproperty::ShaderProperty * prop)
			{
				assert(prop);
				for (unsigned int i = 0; i < m_properties.size(); i++) {
					if (m_properties[i]->equals(*prop)) {	// If equals ->
						m_properties[i] = prop;				// Insert
						return;
					}
				}
				m_properties.push_back(prop);				// Otherwise: append
			}

			void RenderComponent::OnDrawGizmos()
			{/*
				editor::Gizmos::SetMatrix(math::Matrix::CreateWorld(m_bounds.Center, math::Vector3::Forward, math::Vector3::Up));
				editor::Gizmos::SetColor(math::Color(0, 0, 1));
				editor::Gizmos::DrawBoundingOrientedBox(m_bounds);*/
			}

			resource::Model * RenderComponent::GetModel()
			{
				return m_model;
			}
		}
	}
}