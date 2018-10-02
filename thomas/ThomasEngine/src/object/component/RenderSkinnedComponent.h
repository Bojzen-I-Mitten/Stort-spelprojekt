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

		property Model^ model {
			Model^ get();
			void set(Model^ value);
		}
		property Animation^ animation {
			Animation ^get() { return m_anim; }
			void set(Animation^ value);
		}

		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}

		void Update() override;

		Matrix GetBoneMatrix(int boneIndex);

	private:
		void applyAnimation();
	};
}