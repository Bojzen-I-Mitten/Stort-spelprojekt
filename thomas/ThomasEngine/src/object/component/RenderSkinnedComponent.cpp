#pragma once
#pragma unmanaged
#include <thomas\graphics\animation\IBlendTree.h>
#include <thomas\utils\atomic\Synchronization.h>
#include <thomas\object\component\RenderSkinnedComponent.h>
#include <thomas/graphics/animation/AnimationNode.h>
#pragma unmanaged
#include "RenderSkinnedComponent.h"

#include "../../resource/Animation.h"
#include "../../script/animation/BlendNode.h"
#include "../../script/animation/PlaybackNode.h"
#include "../GameObject.h"
#include "Transform.h"

namespace ThomasEngine
{
	RenderSkinnedComponent::RenderSkinnedComponent()
		: RenderComponent(new thomas::object::component::RenderSkinnedComponent())
	{}

	void RenderSkinnedComponent::animation::set(Animation^ value) {
		m_anim = value;
		applyAnimation();
	}


	thomas::object::component::RenderSkinnedComponent* RenderSkinnedComponent::Native::get()
	{
		return (thomas::object::component::RenderSkinnedComponent*)nativePtr;
	}

	thomas::graphics::animation::IBlendTree* RenderSkinnedComponent::BlendTree::get()
	{
		return Native->GetBlendTree();
	}

	void RenderSkinnedComponent::setBlendTreeNode(thomas::graphics::animation::AnimationNode * node)
	{
		if (node == nullptr)
			Native->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr) {
			Native->GetBlendTree()->setBlendTree(node);
		}

	}

	void RenderSkinnedComponent::setBlendTreeNode(Script::BlendNode ^ node)
	{
		if (!node) return;
		setBlendTreeNode(node->Native());
	}

	void RenderSkinnedComponent::setBlendTreeNode(Script::PlaybackNode ^ node)
	{
		if (!node) return;
		setBlendTreeNode(node->Native());
	}

	bool RenderSkinnedComponent::FetchBoneIndex(unsigned int boneHash, unsigned int& boneIndex)
	{
		return BlendTree->getBoneIndex(boneHash, boneIndex);
	}

	bool RenderSkinnedComponent::FetchBoneIndex(unsigned int boneHash, [Out] unsigned int%  boneIndex)
	{
		unsigned int ind;
		bool result =  BlendTree->getBoneIndex(boneHash, ind);
		boneIndex = ind;
		return result;
	}

	void RenderSkinnedComponent::Update()
	{
		Native->Update();
		/*
		if (Input::GetKeyDown(Input::Keys::Space)) {
			thomas::graphics::animation::IBlendTree *anim = ptr->GetBlendTree();
			for (unsigned int i = 0; i < anim->boneCount(); i++) {
				GameObject^ gObj = gcnew GameObject(Utility::ConvertString(anim->getBoneName(i)));
				gObj->transform->world = Utility::Convert(anim->getBoneMatrix(i)) * m_gameObject->transform->world;
			}
		}
		*/
	}

	Matrix RenderSkinnedComponent::GetBoneMatrix(int boneIndex)
	{
		return GetLocalBoneMatrix(boneIndex) * gameObject->transform->world;
	}
	Matrix RenderSkinnedComponent::GetLocalBoneMatrix(int boneIndex)
	{
		thomas::graphics::animation::IBlendTree *anim = Native->GetBlendTree();
		if (!anim && (uint32_t)boneIndex >= anim->boneCount())
			return Matrix::Identity;
		else 
			return Utility::Convert(Native->GetBlendTree()->getBoneOrientation(boneIndex));
	}

	bool RenderSkinnedComponent::GetBoneIndex(unsigned int boneHash, unsigned int & boneIndex)
	{
		if (!Native->GetBlendTree()) return false;
		thomas::graphics::animation::IBlendTree *anim = Native->GetBlendTree();
		return anim->getBoneIndex(boneHash, boneIndex);
	}

	void RenderSkinnedComponent::applyAnimation()
	{
		auto blendTree = ((thomas::object::component::RenderSkinnedComponent*)nativePtr)->GetBlendTree();
		if (!blendTree)
			return;
		if (m_anim == nullptr)
			Native->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr) {
			Native->GetBlendTree()->playSingle(m_anim->Native());
		}
			
	}

}