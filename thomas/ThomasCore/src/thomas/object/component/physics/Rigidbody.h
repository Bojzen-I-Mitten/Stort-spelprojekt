#pragma once
#include "../Component.h"
#include "../../../Physics.h"
#include "CapsuleCollider.h"
namespace thomas
{
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
				void SetActivationState(ActivationState state);
				void SetKinematic(bool kinematic);
				void SetCollider(Collider* collider);
				void SetMass(float mass);

			public:
				float GetMass() const;
				bool IsKinematic() const;
				math::Vector3 GetFreezePosition() const;
				math::Vector3 GetFreezeRotation() const;
				math::Vector3 GetLinearVelocity() const;
				math::Vector3 GetAngularVelocity() const;

			private:
				void UpdateRigidbodyMass();

			private:
				Collider * m_collider = nullptr;
				math::Vector3 m_freezePosition;
				math::Vector3 m_freezeRotation;
				math::Matrix m_prevMatrix;
				float m_mass;
				bool m_kinematic;
			};
		}
	}
}