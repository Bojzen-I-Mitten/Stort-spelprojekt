#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderSkinnedComponent.h>
#include <thomas/graphics/animation/AnimationNode.h>
#include <thomas/graphics/animation/IBlendTree.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../resource/Animation.h"
#include "../../script/animation/BlendNode.h"
using namespace System::Runtime::InteropServices;

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

		thomas::object::component::RenderSkinnedComponent* get();
		thomas::object::component::RenderComponent* getNativeRenderComp();

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

		void setBlendTreeNode(thomas::graphics::animation::AnimationNode * node);

		void setBlendTreeNode(Script::BlendNode^ node);

		bool FetchBoneIndex(uint32_t boneHash, uint32_t& boneIndex);
		bool FetchBoneIndex(uint32_t boneHash, [Out] uint32_t% boneIndex);

		property thomas::graphics::animation::IBlendTree* BlendTree {
			thomas::graphics::animation::IBlendTree* get();
		}
		property thomas::object::component::RenderSkinnedComponent* Native {
			thomas::object::component::RenderSkinnedComponent* get();
		}

		void Update() override;

	private:
		void applyAnimation();
	};
}