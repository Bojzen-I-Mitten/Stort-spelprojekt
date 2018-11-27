#pragma once
#include "RenderComponent.h"

using namespace System::Runtime::InteropServices;
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class RenderSkinnedComponent;
		}
	}
	namespace graphics
	{
		namespace animation
		{
			class IBlendTree;
			class AnimationNode;
		}
	}
}

namespace ThomasEngine
{
	namespace Script
	{
		ref class BlendNode;
		ref class PlaybackNode;
	}

	ref class Animation;
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	public ref class RenderSkinnedComponent : public RenderComponent
	{
	private:
		Animation^ m_anim;
	internal:
		property thomas::graphics::animation::IBlendTree* BlendTree {
			thomas::graphics::animation::IBlendTree* get();
		}
		property thomas::object::component::RenderSkinnedComponent* Native {
			thomas::object::component::RenderSkinnedComponent* get();
		}

		void setBlendTreeNode(thomas::graphics::animation::AnimationNode * node);
	public:
		RenderSkinnedComponent();
		
		property Animation^ animation {
			Animation ^get() { return m_anim; }
			void set(Animation^ value);
		}

		

		void setBlendTreeNode(Script::BlendNode^ node);

		void setBlendTreeNode(Script::PlaybackNode ^ node);
		
		bool FetchBoneIndex(unsigned int boneHash, unsigned int& boneIndex);
		bool FetchBoneIndex(unsigned int boneHash, [Out] unsigned int% boneIndex);

		Matrix GetBoneMatrix(int boneIndex);
		Matrix GetLocalBoneMatrix(int boneIndex);
		bool GetBoneIndex(unsigned int boneHash, unsigned int & boneIndex);

	internal:
		void Update() override;

	private:
		void applyAnimation();
	};
}