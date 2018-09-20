#pragma once

#include "Collider.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class CapsuleCollider : public Collider
			{
			public:
				CapsuleCollider();

				void SetRadius(float size);
				float GetRadius();
				void SetHeight(float height);
				float GetHeight();
				void Update();

				void OnDrawGizmosSelected();
			private:
				float m_radius;
				float m_height;
			};
		}
	}
}