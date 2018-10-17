#include "BoxCollider.h"
#include "../../../editor/gizmos/Gizmos.h"
#include "../../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			BoxCollider::BoxCollider() : Collider(new btBoxShape(btVector3(.5f,.5f,.5f)))
			{
				m_center = math::Vector3(0, 0, 0);
			}

			void BoxCollider::SetSize(math::Vector3 size)
			{
				m_scaling = size*2.0f;
				
			}

			math::Vector3 BoxCollider::getSize()
			{
				return m_scaling * 0.5f;
			}

			void BoxCollider::OnDrawGizmosSelected()
			{
				editor::Gizmos::Gizmo().SetColor(math::Color(0, 1, 0));
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->m_transform->GetWorldMatrix());
				editor::Gizmos::Gizmo().DrawBoundingOrientedBox(DirectX::BoundingOrientedBox(m_center, m_scaling * 0.5f, math::Quaternion::Identity));
				//editor::Gizmos::DrawBoundingCapsule(m_center, 0.5f, 5);
			}

		}
	}
}