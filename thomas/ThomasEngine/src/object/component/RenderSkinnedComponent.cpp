#include "RenderSkinnedComponent.h"
#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#include <thomas\graphics\animation\IBlendTree.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../Input.h"
#include "../../resource/Resources.h"
#include "../GameObject.h"
#include "Transform.h"

namespace ThomasEngine
{
	RenderSkinnedComponent::RenderSkinnedComponent()
		: Component(new thomas::object::component::RenderSkinnedComponent())
	{}

	thomas::object::component::RenderComponent * RenderSkinnedComponent::getNativeRenderComp()
	{
		return (thomas::object::component::RenderComponent*)nativePtr;
	}

	Model^ RenderSkinnedComponent::model::get()
	{
		return m_model;
	}

	void RenderSkinnedComponent::model::set(Model^ value)
	{
		if (value == nullptr)
		{
			getNativeRenderComp()->SetModel(nullptr);
			m_model = nullptr;
		}
		else {
			if (!(getNativeRenderComp())->SetModel(value->Native()))
				m_model = nullptr;
			else
			{
				// Set only if valid model
				m_model = value;
				applyAnimation();
			}
		}
	}

	void RenderSkinnedComponent::animation::set(Animation^ value) {
		m_anim = value;
		applyAnimation();
	}


	Material^ RenderSkinnedComponent::material::get() {
		thomas::resource::Material* nptr = (getNativeRenderComp())->GetMaterial(0);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		else
			return gcnew Material(nptr);
		applyAnimation();
	}
	void RenderSkinnedComponent::material::set(Material^ value) {
		if (value)
			((thomas::object::component::RenderComponent*)nativePtr)->SetMaterial((thomas::resource::Material*)value->m_nativePtr);
		else
			((thomas::object::component::RenderComponent*)nativePtr)->SetMaterial(nullptr);
		OnPropertyChanged("material");
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

	bool RenderSkinnedComponent::FetchBoneIndex(uint32_t boneHash, uint32_t& boneIndex)
	{
		return BlendTree->getBoneIndex(boneHash, boneIndex);
	}

	bool RenderSkinnedComponent::FetchBoneIndex(uint32_t boneHash, [Out] uint32_t%  boneIndex)
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
		assert(Native->GetBlendTree());
		thomas::graphics::animation::IBlendTree *anim = Native->GetBlendTree();
		if ((uint32_t)boneIndex < anim->boneCount())
			return Utility::Convert(anim->getBoneOrientation(boneIndex));
		else return Matrix::Identity;
	}

	bool RenderSkinnedComponent::GetBoneIndex(uint32_t boneHash, uint32_t & boneIndex)
	{
		assert(Native->GetBlendTree());
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