#pragma once
#include "../Component.h"
#include "../../../Physics.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Collider;

			class THOMAS_API Rigidbody : public Component, public btRigidBody
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
				void ApplyCentralForce(const math::Vector3 & force);
				void ApplyForce(const math::Vector3 & force, const math::Vector3 & relPos);
				bool HasCollided(GameObject* collider);

			public:
				void SetKinematic(bool kinematic);
				void SetCollider(btCollisionShape* collider);
				void SetMass(float mass);
				void SetTargetCollider(GameObject* collider);

			public:
				float GetMass();
				bool IsKinematic();

			private:
				void UpdateRigidbodyMass();

			private:
				math::Matrix m_prevMatrix;
				float m_mass;
				bool m_kinematic;
				GameObject* m_targetCollider; // Temp
			};
		}
	}
}