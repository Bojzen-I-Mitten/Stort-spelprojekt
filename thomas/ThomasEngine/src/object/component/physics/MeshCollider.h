#pragma once

#include "Collider.h"

namespace ThomasEngine
{
	ref class Model;
	[ExecuteInEditor]
	public ref class MeshCollider : public Collider
	{
	internal:

	public:
		MeshCollider();
		void Start() override;
		property Model^ mesh
		{
			Model^ get();
			void set(Model^ value);

		}
	};
}