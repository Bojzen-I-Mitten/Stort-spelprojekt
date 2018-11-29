#pragma once

#include "../Component.h"
#include "../../../Physics.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Rigidbody;
			class Collider : public Component
			{
			public:
				typedef void(*OnCollisionEvent)(Collider* otherCollider, Physics::COLLISION_TYPE collisionType);
			public:
				Collider();
				Collider(btCollisionShape* collisionShape);
				~Collider();
				
				void SetCenter(math::Vector3 center);
				math::Vector3 getCenter();
				/*float GetFriction();
				void SetFriction(float value);*/

				Rigidbody* GetAttachedRigidbody();
				void SetAttachedRigidbody(Rigidbody* rb);
				bool IsTrigger();
				virtual void SetTrigger(bool trigger);
				void SetOnCollisionEvent(OnCollisionEvent value);
				void OnCollision(Collider* otherCollider, Physics::COLLISION_TYPE collisionType);
				void OnAwake();
				void OnEnable();
				void OnDisable();
				void OnDestroy();

				virtual void Update();

				btCollisionShape* GetCollisionShape();
				
			protected:
				math::Vector3 m_center;
				math::Vector3 m_scaling;
				bool m_trigger = false;
				btCollisionShape* m_collisionShape = nullptr;
				btCollisionObject* m_collisionObject = nullptr;
				Rigidbody* m_attachedRigidbody = nullptr;
				OnCollisionEvent m_onCollisionEvent = NULL;
			};
		}
	}
}