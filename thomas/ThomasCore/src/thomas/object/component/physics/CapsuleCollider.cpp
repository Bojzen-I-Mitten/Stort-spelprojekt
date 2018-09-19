#include "CapsuleCollider.h"
#include "../../GameObject.h"
#include "../../../editor/gizmos/Gizmos.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			CapsuleCollider::CapsuleCollider() : Collider(new btCapsuleShape(1.0f, 1.0f))
			{
				m_radius = 0.5f;
				m_height = 1.0f;
			}

			void CapsuleCollider::SetRadius(float size)
			{
				m_radius = size >= 0 ? size : 0;

			}

			float CapsuleCollider::GetRadius()
			{
				return m_radius;
			}

			void CapsuleCollider::SetHeight(float height)
			{
				m_height = height >= 0 ? height : 0;

			}

			float CapsuleCollider::GetHeight()
			{
				return m_height;
			}

			void CapsuleCollider::Update()
			{
				math::Vector3 scale = m_gameObject->m_transform->GetScale();
				float xzRadius = scale.x >= scale.z ? scale.x : scale.z;
				scale.x = scale.z = xzRadius;
				scale.x *= m_radius; scale.z *= m_radius;
				scale.y *= m_height;
				m_collisionShape->setLocalScaling((btVector3&)scale);
				
				if (m_collisionObject)
				{
					btTransform trans;
					trans.setOrigin((btVector3&)(m_gameObject->m_transform->GetPosition() + m_center));
					trans.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
					m_collisionObject->setWorldTransform(trans);
					Physics::s_world->updateSingleAabb(m_collisionObject);
				}
			}

			void CapsuleCollider::OnDrawGizmosSelected()
			{
				editor::Gizmos::SetColor(math::Color(0, 1, 0));
				editor::Gizmos::SetMatrix(m_gameObject->m_transform->GetWorldMatrix());
				editor::Gizmos::DrawBoundingCapsule(m_center, m_radius, m_height);
			}
		}
	}
}