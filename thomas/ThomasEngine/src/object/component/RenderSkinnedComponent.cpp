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

namespace ThomasEngine
{
	RenderSkinnedComponent::RenderSkinnedComponent()
		: Component(new thomas::object::component::RenderSkinnedComponent())
	{}

	thomas::object::component::RenderSkinnedComponent * RenderSkinnedComponent::get()
	{
		return (thomas::object::component::RenderSkinnedComponent*)nativePtr;
	}

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
			if (!(getNativeRenderComp())->SetModel(value->get()))
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


	void RenderSkinnedComponent::setBlendTreeNode(thomas::graphics::animation::AnimationNode * node)
	{
		if (node == nullptr)
			get()->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr) {
			std::unique_ptr< thomas::graphics::animation::AnimationNode> n(node);
			get()->GetBlendTree()->setBlendTree(n);
		}

	}

	void RenderSkinnedComponent::setBlendTreeNode(Script::BlendNode ^ node)
	{
		if (!node) return;
		setBlendTreeNode(node->getNativeNode());
	}

	void RenderSkinnedComponent::Update()
	{
		get()->Update();
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

	void RenderSkinnedComponent::applyAnimation()
	{
		if (m_anim == nullptr)
			get()->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr) {
			get()->GetBlendTree()->playSingle(m_anim->get());
		}
			
	}

}