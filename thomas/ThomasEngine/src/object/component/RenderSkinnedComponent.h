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


	/* Animated render component, specialized for rendering skinned models.
	*/
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

		virtual void OnAwake() override;
		/* Animation currently applied directly to the animation tree (set by editor)
		*/
		property Animation^ animation {
			Animation ^get() { return m_anim; }
			void set(Animation^ value);
		}


		/* Set root node in the blending tree.
		*/
		void setBlendTreeNode(Script::BlendNode^ node);
		/* Set root node in the blending tree.
		*/
		void setBlendTreeNode(Script::PlaybackNode ^ node);
		/* Fetch bone index from name hash.
		*/ 
		bool FetchBoneIndex(unsigned int boneHash, unsigned int& boneIndex);
		/* Fetch bone index from name hash.
		*/
		bool FetchBoneIndex(unsigned int boneHash, [Out] unsigned int% boneIndex);
		/* Get the bone matrix in world space.
		*/
		Matrix GetBoneMatrix(int boneIndex);
		/* Get the bone matrix in local space.
		*/
		Matrix GetLocalBoneMatrix(int boneIndex);

	internal:
		void Update() override;

	private:
		void applyAnimation();
	};
}