#pragma once

#include "Collider.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class BoxCollider : public Collider
			{
			public:
				BoxCollider();

				void SetSize(math::Vector3 size);
				math::Vector3 getSize();

				void OnDrawGizmosSelected();
			};
		}
	}
}