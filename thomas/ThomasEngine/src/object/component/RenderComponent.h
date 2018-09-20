#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"

namespace ThomasEngine
{
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	public ref class RenderComponent : public Component
	{
	private:
		Model^ m_model;

	public:
		RenderComponent();
		property Model^ model {
			Model^ get();
			void set(Model^ value);
		}

		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}

		void Update() override;
	};
}