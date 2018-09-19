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
			Rigidbody::Rigidbody() : 
			btRigidBody(1, NULL, NULL), 
			m_targetCollider(nullptr), m_hasGravity(true), 
			m_kinematic(false), 
			m_mass(1.f)
			{
				this->setUserPointer(this);
				Physics::RemoveRigidBody(this);
				btDefaultMotionState* motionState = new btDefaultMotionState();
				setMotionState(motionState);
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

			void Rigidbody::SetGravity(bool gravity)
			{
				if (gravity != m_hasGravity)
				{
					m_hasGravity = gravity;
					if (initialized)
					{
						Physics::RemoveRigidBody(this);
						return m_hasGravity == true ? this->setGravity(Physics::s_world->getGravity()) : this->setGravity(btVector3(0, 0, 0));
						Physics::AddRigidBody(this);
					}
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

			void Rigidbody::AddTorque(const math::Vector3 & torque, ForceMode mode)
			{
				if (mode == ForceMode::Force)
					this->applyTorque(Physics::ToBullet(torque));
				else if (mode == ForceMode::Impulse)
					this->applyTorqueImpulse(Physics::ToBullet(torque));
			}

			void Rigidbody::AddForce(const math::Vector3 & force, ForceMode mode)
			{
				if(mode == ForceMode::Force)
					this->applyCentralForce(Physics::ToBullet(force));
				else if(mode == ForceMode::Impulse)
					this->applyCentralImpulse(Physics::ToBullet(force));
			}

			void Rigidbody::AddRelativeForce(const math::Vector3 & force, const math::Vector3 & relPos, ForceMode mode)
			{
				if (mode == ForceMode::Force)
					this->applyForce(Physics::ToBullet(force), Physics::ToBullet(relPos));
				else if (mode == ForceMode::Impulse)
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

			float Rigidbody::GetMass() const
			{
				return m_mass;
			}

			bool Rigidbody::HasGravity() const
			{
				return m_hasGravity;
			}

			bool Rigidbody::IsKinematic() const
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

