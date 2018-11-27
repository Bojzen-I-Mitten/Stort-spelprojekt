#include "RenderComponent.h"
#include "../../graphics/Mesh.h"
#include "../../resource/Material.h"
#include "../../resource/Model.h"
#include "../GameObject.h"
#include "../../graphics/Renderer.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../resource/ShaderProperty/ShaderProperty.h"
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
				m_materials.push_back(resource::Material::GetStandardMaterial());
			}

			bool RenderComponent::SetModel(resource::Model* model)
			{
				EDITOR_LOCK();
				if (!model)
				{
					m_model = model;
					m_bounds = math::BoundingOrientedBox();
					m_bounds.Extents.x = 0;
					m_bounds.Extents.y = 0;
					m_bounds.Extents.z = 0;
					m_materials.resize(1);
				}
				else
				{
					m_model = model;
					if (m_model->GetMeshes().size() < m_materials.size()){
						m_materials.resize(m_model->GetMeshes().size());
					}
					// Copy existing material to the new meshes
					resource::Material* src_mat = m_materials.size() ? // Find suitable material
						m_materials[0] :
						resource::Material::GetStandardMaterial();
					// Copy
					while (m_model->GetMeshes().size() > m_materials.size())
						m_materials.push_back(src_mat);
				}
				return true;
			}

			void RenderComponent::Update()
			{
				EDITOR_LOCK();
				if (m_model)
				{
					math::BoundingOrientedBox::CreateFromBoundingBox(m_bounds, m_model->m_bounds);
					m_bounds.Transform(m_bounds, m_gameObject->GetTransform()->GetWorldMatrix());

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
				EDITOR_LOCK();
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
				EDITOR_LOCK();
				if (m_materials.size() > meshIndex && meshIndex >= 0)
					return m_materials[meshIndex];
				else
					return resource::Material::GetStandardMaterial();
			}


			void RenderComponent::SubmitToRenderer(Camera* camera)
			{
				EDITOR_LOCK();
				if (m_model)
				{
					// Copy data to frame
					uint32_t num_prop = uint32_t(m_properties.size());
					const thomas::resource::shaderproperty::ShaderPropertyStatic* local_prop;
					if (m_properties.size())
						local_prop = graphics::Renderer::Instance()->getAllocator().m_alloc.allocate(m_properties.data(), num_prop);
					else
						local_prop = nullptr;
					// Submit
					/*if (m_model->GetSkeleton())
					{
						for (int i = 0; i < m_model->GetMeshes().size(); i++)
							SubmitSkinningPart(camera, i, local_prop, num_prop);
					}
					else
					{*/
						for (int i = 0; i < m_model->GetMeshes().size(); i++)
							SubmitPart(camera, i, local_prop, num_prop);
					//}
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


			void RenderComponent::SubmitPart(Camera* camera, unsigned int i, const thomas::resource::shaderproperty::ShaderPropertyStatic* property_data, uint32_t num_prop)
			{
				resource::Material* material = m_materials.size() > i ? m_materials[i] : nullptr;
				if (material == nullptr)
					material = resource::Material::GetStandardMaterial();

				std::shared_ptr<graphics::Mesh> mesh = m_model->GetMeshes()[i];
				
				//assert(verifyPropertyList(m_properties.data(), m_properties.size()));
				
				thomas::graphics::render::RenderCommand cmd(
					m_gameObject->GetTransform()->GetWorldMatrix(),
					mesh.get(), 
					material, 
					camera->ID(), 
					num_prop,
					property_data);

				graphics::Renderer::Instance()->SubmitCommand(cmd);
			}

			resource::shaderproperty::ShaderPropertyStatic & RenderComponent::insertProperty(resource::shaderproperty::ShaderPropertyStatic prop)
			{
				EDITOR_LOCK();
				for (unsigned int i = 0; i < m_properties.size(); i++) {
					if (m_properties[i] ==  prop) {			// If equals ->
						m_properties[i] = prop;				// Insert
						return m_properties[i];
					}
				}
				m_properties.push_back(prop);				// Otherwise: append
				return m_properties.back();
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