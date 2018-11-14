#include "Collider.h"
#include "../../GameObject.h"
#include "Rigidbody.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			Collider::Collider()
			{
				m_scaling = math::Vector3(1.0f, 1.0f, 1.0f);
				m_center = math::Vector3(0, 0, 0);
			}

			Collider::Collider(btCollisionShape * collisionShape)
			{
				m_center = math::Vector3(0, 0, 0);
				m_scaling = math::Vector3(1.0f, 1.0f, 1.0f);
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
				if (trigger) {
					if (m_attachedRigidbody) {
						m_attachedRigidbody->setCollisionFlags(m_attachedRigidbody->getCollisionFlags() | Rigidbody::CF_NO_CONTACT_RESPONSE);
					}
					else if (m_collisionObject) {
						m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() | Rigidbody::CF_NO_CONTACT_RESPONSE);
					}
				}
				else
				{
					if (m_attachedRigidbody) {
						m_attachedRigidbody->setCollisionFlags(m_attachedRigidbody->getCollisionFlags() & ~Rigidbody::CF_NO_CONTACT_RESPONSE);
					}
					else if (m_collisionObject) {
						m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() & ~Rigidbody::CF_NO_CONTACT_RESPONSE);
					}
				}
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

			void Collider::OnEnable()
			{
				if (m_attachedRigidbody == nullptr) //only created if no rigidbody
				{
					m_collisionObject = new btCollisionObject();
					m_collisionShape->setLocalScaling((btVector3&)(m_scaling*m_gameObject->m_transform->GetScale()));
					m_collisionObject->setCollisionShape(m_collisionShape);
					m_collisionObject->setUserPointer(this);
					m_collisionObject->setRestitution(1.0f);

					btTransform trans;
					trans.setOrigin((btVector3&)(m_gameObject->m_transform->GetPosition() + m_center));
					trans.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
					m_collisionObject->setWorldTransform(trans);
					
					SetTrigger(m_trigger); //Update trigger flags
					Physics::s_world->addCollisionObject(m_collisionObject, Physics::GetCollisionGroupBit(m_gameObject->GetLayer()), Physics::GetCollisionMask(m_gameObject->GetLayer()));
							
				
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
				
				m_collisionShape->setLocalScaling((btVector3&)(m_scaling*m_gameObject->m_transform->GetScale()));
				
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

