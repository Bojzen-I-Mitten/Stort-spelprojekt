#include "SphereCollider.h"
#include "../../GameObject.h"
#include "../../../editor/gizmos/Gizmos.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			SphereCollider::SphereCollider() : Collider(new btSphereShape(0.5f))
			{
				m_radius = 0.5f;
			}


			void SphereCollider::SetRadius(float size)
			{
				m_radius = size;
				((btSphereShape*)m_collisionShape)->setUnscaledRadius(m_radius);
				
			}

			float SphereCollider::GetRadius()
			{
				return m_radius;
			}

			void SphereCollider::OnDrawGizmosSelected()
			{

				editor::Gizmos::Gizmo().SetColor(math::Color(0, 1, 0));
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->GetTransform()->GetWorldMatrix());
				editor::Gizmos::Gizmo().DrawBoundingSphere(DirectX::BoundingSphere(m_center, m_radius));
			}

		}
	}
}
