#include "CapsuleCollider.h"
#include "../../GameObject.h"
#include "../../../editor/gizmos/Gizmos.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			CapsuleCollider::CapsuleCollider() : Collider(), m_rotation(ColliderRotation::RotateY)
			{
				m_radius = 0.5f;
				m_height = 1.0f;
			}

			void CapsuleCollider::Awake()
			{
				if (m_rotation == ColliderRotation::RotateX)
					m_collisionShape = new btCapsuleShapeX(1.f, 1.f);
				else if (m_rotation == ColliderRotation::RotateY)
					m_collisionShape = new btCapsuleShape(1.f, 1.f);
				else if (m_rotation == ColliderRotation::RotateZ)
					m_collisionShape = new btCapsuleShapeZ(1.f, 1.f);

				m_collisionShape->setMargin(0);
			}

			void CapsuleCollider::SetRadius(float size)
			{
				m_radius = size >= 0 ? size : 0;
			}

			void CapsuleCollider::SetHeight(float height)
			{
				m_height = height >= 0 ? height : 0;
			}

			void CapsuleCollider::SetRotation(ColliderRotation rotation)
			{
				m_rotation = rotation;
			}

			float CapsuleCollider::GetRadius() const
			{
				return m_radius;
			}

			ColliderRotation CapsuleCollider::GetRotation() const
			{
				return m_rotation;
			}

			float CapsuleCollider::GetHeight() const
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
				math::Matrix rotationMatrix = math::Matrix::Identity;

				if (m_rotation == ColliderRotation::RotateX)
					rotationMatrix = math::Matrix::CreateRotationZ(math::PI / 2.0f);
				else if (m_rotation == ColliderRotation::RotateZ)
					rotationMatrix = math::Matrix::CreateRotationX(math::PI / 2.0f);

				editor::Gizmos::SetMatrix(rotationMatrix * m_gameObject->m_transform->GetWorldMatrix());
				editor::Gizmos::DrawBoundingCapsule(m_center, m_radius, m_height);
			}
		}
	}
}