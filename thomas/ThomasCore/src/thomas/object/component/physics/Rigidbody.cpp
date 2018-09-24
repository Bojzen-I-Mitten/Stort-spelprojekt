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
			m_interpolating(false),
			m_mass(1.f),
			m_freezePosition(1.f),
			m_freezeRotation(1.f)
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
				clearForces();
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

				math::Quaternion rotation = (math::Quaternion)trans.getRotation();
				math::Vector3 pos = (math::Vector3)trans.getOrigin();
				if (m_collider)pos -= math::Vector3::Transform(m_collider->getCenter(), rotation);

				m_gameObject->m_transform->SetPosition(pos);
				m_gameObject->m_transform->SetRotation(rotation);
				m_gameObject->m_transform->SetDirty(true);

				m_prevMatrix = m_gameObject->m_transform->GetWorldMatrix();
			}

			void Rigidbody::UpdateTransformToRigidBody()
			{
				if (m_prevMatrix != m_gameObject->m_transform->GetWorldMatrix())
				{
					btTransform trans;

					math::Vector3 pos = m_gameObject->m_transform->GetPosition();
					math::Quaternion rot = m_gameObject->m_transform->GetRotation();
					if (m_collider)pos += math::Vector3::Transform(m_collider->getCenter(), rot);

					trans.setOrigin((btVector3&)pos);
					trans.setRotation((btQuaternion&)rot);
					getMotionState()->setWorldTransform(trans);
					setCenterOfMassTransform(trans);
					Physics::s_world->updateSingleAabb(this);
					activate();
				}			
			}

			void Rigidbody::SetFreezePosition(const math::Vector3& freezePosition)
			{
				m_freezePosition = freezePosition;
				this->setLinearFactor(Physics::ToBullet(m_freezePosition));
			}

			void Rigidbody::SetFreezeRotation(const math::Vector3& freezeRotation)
			{
				m_freezeRotation = freezeRotation;
				this->setAngularFactor(Physics::ToBullet(m_freezeRotation));	
			}

			void Rigidbody::SetLinearVelocity(const math::Vector3& linearVel)
			{
				m_interpolating ? this->setInterpolationLinearVelocity(Physics::ToBullet(linearVel)) :
								  this->setLinearVelocity(Physics::ToBullet(linearVel));
			}

			void Rigidbody::SetAngularVelocity(const math::Vector3 & angularVel)
			{
				m_interpolating ? this->setInterpolationAngularVelocity(Physics::ToBullet(angularVel)) : 
								  this->setAngularVelocity(Physics::ToBullet(angularVel));
			}

			void Rigidbody::SetActivationState(ActivationState state)
			{
				this->setActivationState(state);
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

			void Rigidbody::SetInterpolation(bool interpolate)
			{
				m_interpolating = interpolate;
			}
	
			void Rigidbody::SetCollider(Collider * collider)
			{
				m_collider = collider;
				bool removed = Physics::RemoveRigidBody(this);
				delete getCollisionShape();
				setCollisionShape(collider->GetCollisionShape());
				UpdateRigidbodyMass();
				if(removed)
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

			void Rigidbody::ClearTargetCollider()
			{
				m_targetCollider.reset();
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

			bool Rigidbody::IsInterpolating() const
			{
				return m_interpolating;
			}

			math::Vector3 Rigidbody::GetFreezePosition() const
			{
				return m_freezePosition;
			}

			math::Vector3 Rigidbody::GetFreezeRotation() const
			{
				return m_freezeRotation;
			}

			math::Vector3 Rigidbody::GetLinearVelocity() const
			{
				if (m_interpolating)
					return Physics::ToSimple(this->getInterpolationLinearVelocity());

				return Physics::ToSimple(this->getLinearVelocity());
			}

			math::Vector3 Rigidbody::GetAngularVelocity() const
			{
				if (m_interpolating)
					return Physics::ToSimple(this->getInterpolationAngularVelocity());

				return Physics::ToSimple(this->getAngularVelocity());
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

