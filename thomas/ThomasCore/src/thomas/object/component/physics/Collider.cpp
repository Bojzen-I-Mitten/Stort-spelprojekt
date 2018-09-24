#include "Collider.h"
#include "../../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			Collider::Collider(btCollisionShape * collisionShape)
			{
				m_center = math::Vector3(0, 0, 0);
				m_collisionShape = collisionShape;
				m_collisionShape->setMargin(0);
			}

			Collider::~Collider()
			{
				
			}
			void Collider::SetCenter(math::Vector3 center)
			{
				m_center = center;
			}

			math::Vector3 Collider::getCenter()
			{
				return m_center;
			}

			Rigidbody * Collider::GetAttachedRigidbody()
			{
				return m_attachedRigidbody;
			}

			void Collider::SetAttachedRigidbody(Rigidbody * rb)
			{
				m_attachedRigidbody = rb;
			}

			bool Collider::IsTrigger()
			{
				return m_trigger;
			}

			void Collider::SetTrigger(bool trigger)
			{
				m_trigger = trigger;
			}

			void Collider::SetOnCollisionEvent(OnCollisionEvent value)
			{
				m_onCollisionEvent = value;
			}

			void Collider::OnCollision(Collider * otherCollider, Physics::COLLISION_TYPE collisionType)
			{
				if (m_onCollisionEvent)
					m_onCollisionEvent(otherCollider, collisionType);
			}

			void Collider::Awake()
			{
			}

			void Collider::OnEnable() //maybe later
			{
				if (m_attachedRigidbody == nullptr) //only created if no rigidbody
				{
					m_collisionObject = new btCollisionObject();
					m_collisionObject->setCollisionShape(m_collisionShape);
					m_collisionObject->setUserPointer(this);
					Physics::s_world->addCollisionObject(m_collisionObject);
				}
			}

			void Collider::OnDisable()
			{
				if (m_collisionObject)
				{
					Physics::s_world->removeCollisionObject(m_collisionObject);
					m_collisionObject = nullptr;
				}
			}

			void Collider::OnDestroy()
			{
				OnDisable();
				//SAFE_DELETE(m_collisionShape);
			}

			void Collider::Update()
			{
				
				m_collisionShape->setLocalScaling((btVector3&)m_gameObject->m_transform->GetScale());
				
				if (m_collisionObject)
				{
					btTransform trans;
					trans.setOrigin((btVector3&)(m_gameObject->m_transform->GetPosition() + m_center));
					trans.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
					m_collisionObject->setWorldTransform(trans);
					Physics::s_world->updateSingleAabb(m_collisionObject);
				}
			}

			btCollisionShape * Collider::GetCollisionShape()
			{
				return m_collisionShape;
			}

		}
	}
}

