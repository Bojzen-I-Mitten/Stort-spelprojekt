#include "Rigidbody.h"
#include "../../GameObject.h"
#include "../../../utils/Math.h"
#include "../Transform.h"
#include "Collider.h"
#include <imgui/ImGuizmo.h>
#include <memory>

namespace thomas
{
	namespace object
	{
		namespace component
		{
			Rigidbody::Rigidbody() : 
			btRigidBody(1, NULL, NULL), 
			m_kinematic(false),
			m_mass(1.f),
			m_freezePosition(1.f),
			m_freezeRotation(1.f),
			m_activationState(ActivationState::Default)
			{
		/*		setDamping(0.05, 0.85);
				setDeactivationTime(3.0);
				setSleepingThresholds(5.6, 5.5);
				setContactProcessingThreshold(0.25f);
				setCcdMotionThreshold(0.05f);
				setCcdSweptSphereRadius(0.06f);*/
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
				this->setLinearVelocity(btVector3(0, 0, 0));
				this->setAngularVelocity(btVector3(0, 0, 0));
				Physics::AddRigidBody(this);
			}

			void Rigidbody::OnDisable()
			{
				this->setLinearVelocity(btVector3(0, 0, 0));
				this->setAngularVelocity(btVector3(0, 0, 0));
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

				m_prevMatrix = m_gameObject->m_transform->GetLocalWorldMatrix();
			}

			void Rigidbody::UpdateTransformToRigidBody()
			{
				if (m_prevMatrix != m_gameObject->m_transform->GetLocalWorldMatrix())
				{
					btTransform trans;

					math::Vector3 pos = m_gameObject->m_transform->GetPosition();
					math::Quaternion rot = m_gameObject->m_transform->GetRotation();
					if (m_collider)pos += math::Vector3::Transform(m_collider->getCenter(), rot);

					trans.setRotation((btQuaternion&)rot);
					getMotionState()->setWorldTransform(trans);

					setCenterOfMassTransform(trans);
					trans.setOrigin((btVector3&)pos);
					getMotionState()->setWorldTransform(trans);

					if (ImGuizmo::IsUsing()) {
						this->setLinearVelocity(btVector3(0, 0, 0));
						this->setAngularVelocity(btVector3(0, 0, 0));
					}
					trans.setOrigin((btVector3&)(pos + m_LocalCenterOfMassChange));
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
				this->setLinearVelocity(Physics::ToBullet(linearVel));
			}

			void Rigidbody::SetAngularVelocity(const math::Vector3& angularVel)
			{
				this->setAngularVelocity(Physics::ToBullet(angularVel));
			}

			void Rigidbody::SetDamping(const math::Vector2& damping)
			{
				m_damping = damping;
				this->setDamping(damping.x, damping.y);
			}

			void Rigidbody::SetSleepingThresholds(const math::Vector2 & thresholds)
			{
				m_sleepingThresholds = thresholds;
				this->setSleepingThresholds(m_sleepingThresholds.x, m_sleepingThresholds.y);
			}

			void Rigidbody::SetDeactivationTime(float deactivationTime)
			{
				this->setDeactivationTime(deactivationTime);
			}

			void Rigidbody::SetContactProcessingThreshold(float contactProcessingThreshold)
			{	
				this->setContactProcessingThreshold(contactProcessingThreshold);
			}

			void Rigidbody::SetCcdMotionThreshold(float motionThreshold)
			{
				this->setCcdMotionThreshold(motionThreshold);
			}

			void Rigidbody::SetCcdSweptSphereRadius(float sphereRadius)
			{
				this->setCcdSweptSphereRadius(sphereRadius);
			}

			void Rigidbody::SetActivationState(ActivationState state)
			{
				m_activationState = state;
				this->setActivationState(m_activationState);
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

			void Rigidbody::SetCollider(Collider * collider)
			{
				m_collider = collider;
				bool removed = Physics::RemoveRigidBody(this);
				delete getCollisionShape();
				setCollisionShape(collider->GetCollisionShape());
				this->setUserPointer(collider);
				collider->SetAttachedRigidbody(this);
				collider->SetTrigger(collider->IsTrigger());
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

			void Rigidbody::SetCenterOfmass(math::Vector3 Centerofmass)
			{
				
				m_LocalCenterOfMassChange = Centerofmass;
			}

			math::Vector3 Rigidbody::GetCenterOfmass()
			{
				
				return m_LocalCenterOfMassChange;
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
						
			float Rigidbody::GetMass() const
			{
				return m_mass;
			}

			bool Rigidbody::IsKinematic() const
			{
				return m_kinematic;
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
				return Physics::ToSimple(this->getLinearVelocity());
			}

			math::Vector3 Rigidbody::GetAngularVelocity() const
			{
				return Physics::ToSimple(this->getAngularVelocity());
			}

			math::Vector2 Rigidbody::GetDamping() const
			{
				return m_damping;
			}

			math::Vector2 Rigidbody::GetSleepingThresholds() const
			{
				return m_sleepingThresholds;
			}

			float Rigidbody::GetCcdSweptSphereRadius() const
			{
				return this->getCcdSweptSphereRadius();
			}

			float Rigidbody::GetDeactivationTime() const
			{
				return this->getDeactivationTime();
			}

			float Rigidbody::GetContactProcessingThreshold() const
			{
				return this->getContactProcessingThreshold();
			}

			float Rigidbody::GetCcdMotionThreshold() const
			{
				return this->getCcdMotionThreshold();
			}

			ActivationState Rigidbody::GetActivationState() const
			{
				return m_activationState;
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

