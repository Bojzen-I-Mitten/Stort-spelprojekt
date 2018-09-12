#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderSkinnedComponent.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"

namespace ThomasEngine
{
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	public ref class RenderSkinnedComponent : public Component
	{
	private:
		Model^ m_model;

	public:
		RenderSkinnedComponent();

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