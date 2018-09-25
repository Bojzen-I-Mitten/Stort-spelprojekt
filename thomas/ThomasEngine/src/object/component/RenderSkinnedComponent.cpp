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

#include "../GameObject.h"

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
		if (value == nullptr)
		{
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetModel(nullptr);
			m_model = nullptr;
		}
		else {
			if (!((thomas::object::component::RenderSkinnedComponent*)nativePtr)->SetModel((thomas::resource::Model*)value->m_nativePtr))
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
		thomas::resource::Material* nptr = ((thomas::object::component::RenderComponent*)nativePtr)->GetMaterial(0);
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


	void RenderSkinnedComponent::Update()
	{
		thomas::object::component::RenderSkinnedComponent* ptr = ((thomas::object::component::RenderSkinnedComponent*)nativePtr);
		ptr->Update();
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
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->GetBlendTree()->clearBlendTree();
		else if (m_model != nullptr) {
			((thomas::object::component::RenderSkinnedComponent*)nativePtr)->GetBlendTree()->playSingle(m_anim->get());
		}
			
	}

}