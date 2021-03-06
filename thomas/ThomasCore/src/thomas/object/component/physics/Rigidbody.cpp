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
				m_useGravity(true),
				m_mass(1.f),
				m_freezePosition(1.f),
				m_freezeRotation(1.f),
				m_damping(0.0f),
				m_angularDaming(0.0f),
				m_dirty(false),
				m_ignoreTransform(false),
				m_LocalCenterOfMassChange(0, 0, 0),
				m_activationState(ActivationState::Default)
			{
				m_bounciness = m_restitution;
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
				math::Vector3 pos = m_gameObject->GetTransform()->GetPosition();
				math::Quaternion rot = m_gameObject->GetTransform()->GetRotation();
				if (m_collider)pos += math::Vector3::Transform(m_collider->getCenter(), rot);

				trans.setRotation((btQuaternion&)rot);
				trans.setOrigin((btVector3&)pos);

				setWorldTransform(trans);
				getMotionState()->setWorldTransform(trans);
				setCenterOfMassTransform(trans);
				UpdateRigidbodyMass();

				this->setUserPointer(m_collider);

				this->setLinearVelocity(btVector3(0, 0, 0));
				this->setAngularVelocity(btVector3(0, 0, 0));
				clearForces();
				
				Physics::AddRigidBody(this);
				m_prevMatrix = m_gameObject->GetTransform()->GetWorldMatrix();
				UpdateProperties();
				UpdateRigidBodyTransform();	// Reset transform
			}

			void Rigidbody::OnDisable()
			{
				this->setLinearVelocity(btVector3(0, 0, 0));
				this->setAngularVelocity(btVector3(0, 0, 0));
				clearForces();
				this->setUserPointer(nullptr);
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


				m_gameObject->GetTransform()->SetRotation(rotation);
				m_gameObject->GetTransform()->SetPosition(pos);
				m_gameObject->GetTransform()->SetDirty(true);

				m_prevMatrix = m_gameObject->GetTransform()->GetWorldMatrix();

				if (m_dirty)
				{
					UpdateProperties();
				}


			}

			void Rigidbody::UpdateRigidBodyTransform()
			{
				btTransform trans;

				math::Vector3 pos = m_gameObject->GetTransform()->GetPosition();
				math::Quaternion rot = m_gameObject->GetTransform()->GetRotation();
				if (m_collider)pos += math::Vector3::Transform(m_collider->getCenter(), rot);

				trans.setRotation((btQuaternion&)rot);
				trans.setOrigin((btVector3&)pos);
				//getMotionState()->setWorldTransform(trans);

#ifdef _EDITOR
				if (ImGuizmo::IsUsing()) {
					this->setLinearVelocity(btVector3(0, 0, 0));
					this->setAngularVelocity(btVector3(0, 0, 0));
				}
#endif
				//trans.setOrigin((btVector3&)(pos + m_LocalCenterOfMassChange));
				//setCenterOfMassTransform(trans);


				setWorldTransform(trans);
				getMotionState()->setWorldTransform(trans);
				setCenterOfMassTransform(trans);
				Physics::s_world->updateSingleAabb(this);
				activate();
			}

			void Rigidbody::UpdateTransformToRigidBody()
			{
				math::Matrix currentMatrix = m_gameObject->GetTransform()->GetWorldMatrix();
				// Verify not changed and that it shouldn't be skipped for the frame.
				if (!m_ignoreTransform && m_prevMatrix != currentMatrix)
				{
					// Update rigid body transform
					UpdateRigidBodyTransform();
				}
				m_prevMatrix = currentMatrix;
				m_ignoreTransform = false;
			}

			void Rigidbody::SetFreezePosition(const math::Vector3& freezePosition)
			{
				m_freezePosition = freezePosition;
				m_dirty = true;
			}

			void Rigidbody::SetFreezeRotation(const math::Vector3& freezeRotation)
			{
				m_freezeRotation = freezeRotation;
				m_dirty = true;
			}

			void Rigidbody::SetLinearVelocity(const math::Vector3& linearVel)
			{
				//if (initialized)
					this->setLinearVelocity(Physics::ToBullet(linearVel));
			}

			void Rigidbody::SetAngularVelocity(const math::Vector3& angularVel)
			{
				//if (initialized)
					this->setAngularVelocity(Physics::ToBullet(angularVel));
			}

			void Rigidbody::SetDamping(float damping)
			{
				m_damping = damping;
				m_dirty = true;
			}

			void Rigidbody::SetAngularDamping(float angularDamping)
			{
				m_angularDaming = angularDamping;
				m_dirty = true;
			}

			void Rigidbody::SetSleepingThresholds(const math::Vector2 & thresholds)
			{
				m_sleepingThresholds = thresholds;
				m_dirty = true;
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
				m_dirty = true;
			}

			void Rigidbody::SetKinematic(bool kinematic)
			{
				if (kinematic != m_kinematic)
				{
					m_kinematic = kinematic;
					
					if (m_enabled)
					{
						bool removed = Physics::RemoveRigidBody(this);
						UpdateRigidbodyMass();
						if (removed)
							Physics::AddRigidBody(this);
					}
				}
			}

			void Rigidbody::UseGravity(bool value)
			{
				m_useGravity = value;
				m_dirty = true;
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
				if (removed)
					Physics::AddRigidBody(this);
			}

			void Rigidbody::SetMass(float mass)
			{
				m_mass = mass;
				if (m_enabled)
				{
					bool removed = Physics::RemoveRigidBody(this);
					UpdateRigidbodyMass();
					if (removed)
						Physics::AddRigidBody(this);
				}
			}

			void Rigidbody::SetCenterOfmass(math::Vector3 Centerofmass)
			{

				m_LocalCenterOfMassChange = Centerofmass;
			}

			void Rigidbody::SetBounciness(float bounciness)
			{
				m_restitution = bounciness;
			}

			void Rigidbody::SetFriction(float friction)
			{
				m_friction = friction;
				m_rollingFriction = friction;
			}

			void Rigidbody::SetPosition(math::Vector3 position)
			{
				btTransform trans = getCenterOfMassTransform();
				trans.setOrigin(Physics::ToBullet(position));
				setCenterOfMassTransform(trans);


			}

			void Rigidbody::SetRotation(math::Quaternion rotation)
			{

				btTransform trans = getCenterOfMassTransform();
				trans.setRotation(Physics::ToBullet(rotation));
				setCenterOfMassTransform(trans);


			}


			void Rigidbody::IgnoreNextTransformUpdate()
			{
				m_ignoreTransform = true;
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
				if (mode == ForceMode::Force)
					this->applyCentralForce(Physics::ToBullet(force));
				else if (mode == ForceMode::Impulse)
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

			bool Rigidbody::UsesGravity() const
			{
				return m_useGravity;
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

			float Rigidbody::GetDamping() const
			{
				return m_damping;
			}

			float Rigidbody::GetAngularDamping() const
			{
				return m_angularDaming;
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

			float Rigidbody::GetBounciness()
			{
				return m_restitution;
			}

			float Rigidbody::GetFriction()
			{
				return m_friction;
			}

			math::Vector3 Rigidbody::GetPosition()
			{
				btTransform trans = getCenterOfMassTransform();
				getMotionState()->getWorldTransform(trans);
				return Physics::ToSimple(trans.getOrigin());
			}

			math::Quaternion Rigidbody::GetRotation()
			{
				btTransform trans = getCenterOfMassTransform();
				getMotionState()->getWorldTransform(trans);
				return Physics::ToSimple(trans.getRotation());
			}


			void Rigidbody::UpdateRigidbodyMass()
			{
				float mass = m_kinematic ? 0 : m_mass;
				btVector3 inertia = getLocalInertia();

				if (getCollisionShape())
					getCollisionShape()->calculateLocalInertia(mass, inertia);

				setMassProps(mass, inertia);
				updateInertiaTensor();
				this->activate();
			}

			void Rigidbody::UpdateProperties()
			{
				this->setActivationState(m_activationState);
				this->setLinearFactor(Physics::ToBullet(m_freezePosition));
				this->setAngularFactor(Physics::ToBullet(m_freezeRotation));
				this->setDamping(m_damping, m_angularDaming);
				this->setSleepingThresholds(m_sleepingThresholds.x, m_sleepingThresholds.y);
				this->setGravity(Physics::s_world->getGravity() * m_useGravity);
				this->activate(true);

				m_dirty = false;
			}
		}
	}
}