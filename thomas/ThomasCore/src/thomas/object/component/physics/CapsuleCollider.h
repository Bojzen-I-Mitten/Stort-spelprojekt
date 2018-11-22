#pragma once

#include "Collider.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			enum class ColliderRotation
			{
				RotateX,
				RotateY,
				RotateZ
			};

			class CapsuleCollider : public Collider
			{
			public:
				CapsuleCollider();
				void OnAwake();
				void Update();
				void OnDrawGizmosSelected();

			public:
				void SetRadius(float size);
				void SetHeight(float height);
				void SetRotation(ColliderRotation rotation);

			public:
				float GetRadius() const;	
				float GetHeight() const;
				ColliderRotation GetRotation() const;

			private:
				ColliderRotation m_rotation;
				float m_radius;
				float m_height;
			};
		}
	}
}