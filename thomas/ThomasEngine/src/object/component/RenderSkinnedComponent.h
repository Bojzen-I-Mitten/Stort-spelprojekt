#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderSkinnedComponent.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../resource/Animation.h"

namespace ThomasEngine
{
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	public ref class RenderSkinnedComponent : public Component
	{
	private:
		Model^ m_model;
		Animation^ m_anim;
	public:
		RenderSkinnedComponent();

		[DataMember(Order = 2)]
		property Model^ model {
			Model^ get();
			void set(Model^ value);
		}
		[DataMember(Order = 3)]
		property Animation^ animation {
			Animation ^get() { return m_anim; }
			void set(Animation^ value);
		}


		[DataMember(Order = 4)]
		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}

		void Update() override;

	private:
		void applyAnimation();
	};
}