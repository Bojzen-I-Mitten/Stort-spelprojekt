#pragma once

#include "Collider.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class SphereCollider : public Collider
			{
			public:
				SphereCollider();

				void SetRadius(float size);
				float GetRadius();

				void OnDrawGizmosSelected();
			private:
				float m_radius;
			};
		}
	}
}