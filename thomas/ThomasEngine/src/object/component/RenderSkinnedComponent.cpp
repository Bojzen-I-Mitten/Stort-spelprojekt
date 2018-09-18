#include "RenderSkinnedComponent.h"
#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#include <thomas\graphics\animation\IBlendTree.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"

namespace ThomasEngine
{
	RenderSkinnedComponent::RenderSkinnedComponent()
		: Component(new thomas::object::component::RenderSkinnedComponent())
	{}

	Model^ RenderSkinnedComponent::model::get()
	{
		return m_model;
	}

	void RenderSkinnedComponent::model::set(Model^ value)
	{
		m_model = value;
		if (m_model == nullptr)
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetModel(nullptr);
		else
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetModel((thomas::resource::Model*)value->m_nativePtr);
		applyAnimation();
	}

	void RenderSkinnedComponent::animation::set(Animation^ value) {
		m_anim = value;
		applyAnimation();
	}


	Material^ RenderSkinnedComponent::material::get() {
		thomas::resource::Material* nptr = ((thomas::object::component::RenderComponent*)nativePtr)->GetMaterial(0);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		else
			return gcnew Material(nptr);
	}
	void RenderSkinnedComponent::material::set(Material^ value) {
		if (value)
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetMaterial(0, (thomas::resource::Material*)value->m_nativePtr);
		else
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetMaterial(0, nullptr);

		OnPropertyChanged("material");
	}


	void RenderSkinnedComponent::Update()
	{
		((thomas::object::component::RenderSkinnedComponent*)nativePtr)->Update();
	}

	void RenderSkinnedComponent::applyAnimation()
	{
		if (m_anim == nullptr)
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr)
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->GetBlendTree()->playSingle(m_anim->get());
	}

}