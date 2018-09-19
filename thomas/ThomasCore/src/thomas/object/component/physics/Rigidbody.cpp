#include "Rigidbody.h"
#include "../../GameObject.h"
#include "../../../utils/Math.h"
#include "../Transform.h"
#include "Collider.h"
#include <memory>

namespace thomas
{
	namespace object
	{
		namespace component
		{
			Rigidbody::Rigidbody() : btRigidBody(1, NULL, NULL), m_targetCollider(nullptr)
			{
				this->setUserPointer(this);
				Physics::RemoveRigidBody(this);
				btDefaultMotionState* motionState = new btDefaultMotionState();
				setMotionState(motionState);
				m_kinematic = false;
				m_mass = 1;
			}
			
			Rigidbody::~Rigidbody()
			{
				Physics::RemoveRigidBody(this);
				delete getMotionState();
				
				Physics::s_world->removeCollisionObject(this);
				delete getCollisionShape();				
			}

			void Rigidbody::OnEnable()
			{
				btTransform trans;
				trans.setFromOpenGLMatrix(*m_gameObject->m_transform->GetWorldMatrix().m);
				getMotionState()->setWorldTransform(trans);
				setCenterOfMassTransform(trans);
				UpdateRigidbodyMass();
				Physics::AddRigidBody(this);
			}

			void Rigidbody::OnDisable()
			{
				Physics::RemoveRigidBody(this);				
			}

			void Rigidbody::OnDestroy()
			{
				OnDisable();
			}

			void Rigidbody::UpdateRigidbodyToTransform()
			{			
				btTransform trans;
				getMotionState()->getWorldTransform(trans);
				m_gameObject->m_transform->SetPosition((math::Vector3)trans.getOrigin());
				m_gameObject->m_transform->SetRotation((math::Quaternion)trans.getRotation());
				m_gameObject->m_transform->SetDirty(true);

				m_prevMatrix = m_gameObject->m_transform->GetWorldMatrix();
			}

			void Rigidbody::UpdateTransformToRigidBody()
			{
				if (m_prevMatrix != m_gameObject->m_transform->GetWorldMatrix())
				{
					btTransform trans;
					trans.setOrigin((btVector3&)m_gameObject->m_transform->GetPosition());
					trans.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
					getMotionState()->setWorldTransform(trans);
					setCenterOfMassTransform(trans);
					Physics::s_world->updateSingleAabb(this);
					activate();
				}			
			}

			void Rigidbody::SetKinematic(bool kinematic)
			{	
				if (kinematic != m_kinematic)
				{
					m_kinematic = kinematic;
					if (initialized)
					{
						Physics::RemoveRigidBody(this);
						UpdateRigidbodyMass();
						Physics::AddRigidBody(this);
					}		
				}	
			}
	
			void Rigidbody::SetCollider(btCollisionShape * collider)
			{
				Physics::RemoveRigidBody(this);
				delete getCollisionShape();
				setCollisionShape(collider);
				UpdateRigidbodyMass();
				Physics::AddRigidBody(this);
			}

			void Rigidbody::SetMass(float mass)
			{
				m_mass = mass;
				if (initialized)
				{
					Physics::RemoveRigidBody(this);
					UpdateRigidbodyMass();
					Physics::AddRigidBody(this);				
				}
			}

			void Rigidbody::SetTargetCollider(GameObject* collider)
			{
				if (m_targetCollider == nullptr)
				{
					m_targetCollider = std::make_unique<GameObject>("");					
				}

				// Don't change the pointer if target collider has not been updated
				if(m_targetCollider.get() != collider)
					*m_targetCollider = *collider;
			}

			void Rigidbody::ApplyCentralForce(const math::Vector3 & force)
			{
				this->applyCentralForce(Physics::ToBullet(force));
			}

			void Rigidbody::ApplyCentralImpulseForce(const math::Vector3 & force)
			{
				this->applyCentralImpulse(Physics::ToBullet(force));
			}

			void Rigidbody::ApplyForce(const math::Vector3 & force, const math::Vector3 & relPos)
			{
				this->applyForce(Physics::ToBullet(force), Physics::ToBullet(relPos));
			}

			void Rigidbody::ApplyImpulseForce(const math::Vector3 & force, const math::Vector3 & relPos)
			{
				this->applyImpulse(Physics::ToBullet(force), Physics::ToBullet(relPos));
			}

			GameObject * Rigidbody::GetTargetCollider()
			{
				if (m_targetCollider != nullptr)
				{
					if (this->hasContactResponse() && m_targetCollider->GetComponent<object::component::Rigidbody>()->hasContactResponse())
						return m_targetCollider.get();
				}

				return nullptr;
			}

			float Rigidbody::GetMass()
			{
				return m_mass;
			}

			bool Rigidbody::IsKinematic()
			{
				return m_kinematic;
			}

			void Rigidbody::UpdateRigidbodyMass()
			{
				float mass = m_kinematic ? 0 : m_mass;
				btVector3 inertia = getLocalInertia();

				if (getCollisionShape())
					getCollisionShape()->calculateLocalInertia(mass, inertia);

				setMassProps(mass, inertia);
				updateInertiaTensor();
			}
		}
	}
}

