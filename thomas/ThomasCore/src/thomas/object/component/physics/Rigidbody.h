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
				struct Collision
				{
					Rigidbody* thisRigidbody;
					Rigidbody* otherRigidbody;
				};

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
				bool OnCollisionEnter(const std::string & name);

			public:
				void SetKinematic(bool kinematic);
				void SetCollider(btCollisionShape* collider);
				void SetMass(float mass);

			public:
				float GetMass();
				bool IsKinematic();

			private:
				void UpdateRigidbodyMass();

			private:
				math::Matrix m_prevMatrix;
				float m_mass;
				bool m_kinematic;
				bool m_collided;
			};
		}
	}
}