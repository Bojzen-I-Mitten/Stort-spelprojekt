#pragma once
#include "../Component.h"
#include "../../../Physics.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
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
				void AddTorque(const math::Vector3 & torque, ForceMode mode = ForceMode::Force);
				void AddForce(const math::Vector3 & force, ForceMode mode = ForceMode::Force);
				void AddRelativeForce(const math::Vector3 & force, const math::Vector3 & relPos, ForceMode mode = ForceMode::Force);

			public:
				void SetFreezePosition(const math::Vector3 & freezePosition);
				void SetFreezeRotation(const math::Vector3 & freezeRotation);
				void SetGravity(bool gravity);
				void SetKinematic(bool kinematic);
				void SetCollider(Collider* collider);
				void SetMass(float mass);
				void SetTargetCollider(GameObject* collider);

			public:
				GameObject* GetTargetCollider();
				float GetMass() const;
				bool HasGravity() const;
				bool IsKinematic() const;
				math::Vector3 GetFreezePosition() const;
				math::Vector3 GetFreezeRotation() const;
				void ClearTargetCollider();

			private:
				void UpdateRigidbodyMass();

			private:
				Collider * m_collider = nullptr;
				math::Vector3 m_freezePosition;
				math::Vector3 m_freezeRotation;
				math::Matrix m_prevMatrix;
				float m_mass;
				bool m_kinematic;
				bool m_hasGravity;
				std::unique_ptr<GameObject> m_targetCollider; // Temp
			};
		}
	}
}