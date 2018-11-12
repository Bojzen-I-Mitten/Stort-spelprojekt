#pragma once
#include "../Component.h"
#include "../../../Physics.h"
#include "CapsuleCollider.h"
namespace thomas
{
	namespace graphics
	{
		namespace animation
		{
			class BoneConstraint;
		}
	}
	namespace object
	{
		namespace component
		{


			enum ActivationState
			{
				Default = WANTS_DEACTIVATION,
				Sleeping = ISLAND_SLEEPING,
				Disabled = DISABLE_SIMULATION,
				Active_Tag = ACTIVE_TAG,
				Always_Active = DISABLE_DEACTIVATION	
			};

			enum class ForceMode
			{
				Force,
				Impulse, 
			};

			class Collider;
			class Rigidbody : public Component, public btRigidBody
			{
			public:
				Rigidbody();
				~Rigidbody();

			public:
				void OnEnable();
				void OnDisable();
				void OnDestroy();
				void UpdateRigidbodyToTransform();
				void UpdateTransformToRigidBody();
				void AddTorque(const math::Vector3& torque, ForceMode mode = ForceMode::Force);
				void AddForce(const math::Vector3& force, ForceMode mode = ForceMode::Force);
				void AddRelativeForce(const math::Vector3& force, const math::Vector3 & relPos, ForceMode mode = ForceMode::Force);

			public:
				void SetFreezePosition(const math::Vector3& freezePosition);
				void SetFreezeRotation(const math::Vector3& freezeRotation);
				void SetLinearVelocity(const math::Vector3& linearVel);
				void SetAngularVelocity(const math::Vector3& angularVel);
				void SetDamping(float damping);
				void SetAngularDamping(float angularDamping);
				void SetSleepingThresholds(const math::Vector2& thresholds);
				void SetDeactivationTime(float deactivationTime);
				void SetContactProcessingThreshold(float contactProcessingThreshold);
				void SetCcdMotionThreshold(float motionThreshold);
				void SetCcdSweptSphereRadius(float sphereRadius);
				void SetActivationState(ActivationState state);
				void SetKinematic(bool kinematic);
				void UseGravity(bool value);
				void SetCollider(Collider* collider);
				void SetMass(float mass);
				void SetCenterOfmass(math::Vector3 Centerofmass);
				void SetBounciness(float bounciness);
				void SetFriction(float friction);
				void SetPosition(math::Vector3 position);
				void SetRotation(math::Quaternion rotation);

				void DisableRotationSync();

			public:
				float GetMass() const;
				bool IsKinematic() const;
				bool UsesGravity() const;
				math::Vector3 GetFreezePosition() const;
				math::Vector3 GetFreezeRotation() const;
				math::Vector3 GetLinearVelocity() const;
				math::Vector3 GetAngularVelocity() const;
				float GetDamping() const;
				float GetAngularDamping() const;
				math::Vector2 GetSleepingThresholds() const;
				float GetDeactivationTime() const;
				float GetContactProcessingThreshold() const;
				float GetCcdMotionThreshold() const;
				float GetCcdSweptSphereRadius() const;
				ActivationState GetActivationState() const;
				math::Vector3 GetCenterOfmass();
				float GetBounciness();
				float GetFriction();
				math::Vector3 GetPosition();
				math::Quaternion GetRotation();
			private:
				void UpdateRigidbodyMass();
				void UpdateProperties();
			private:
				Collider * m_collider = nullptr;
				math::Vector3 m_freezePosition;
				math::Vector3 m_freezeRotation;
				math::Vector2 m_sleepingThresholds;
				math::Matrix m_prevMatrix;
				ActivationState m_activationState;
				math::Vector3 m_LocalCenterOfMassChange;
				float m_mass;
				float m_damping;
				float m_angularDaming;
				bool m_kinematic;
				bool m_useGravity;
				float m_bounciness;
				bool m_dirty;
				bool m_syncRotation;
			};
		}
	}
}