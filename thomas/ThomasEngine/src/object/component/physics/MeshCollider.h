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

		property bool concave
		{
			bool get();
			void set(bool value);
		}

		property float margin
		{
			float get();
			void set(float value);
		}
	};
}