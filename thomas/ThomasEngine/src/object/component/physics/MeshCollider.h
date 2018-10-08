#pragma once

#include "Collider.h"

namespace ThomasEngine
{
	ref class Model;
	public ref class MeshCollider : public Collider
	{
	internal:
		void OnGameObjectSet() override;
	public:
		MeshCollider();
		property Model^ mesh
		{
			Model^ get();
			void set(Model^ value);

		}
	};
}