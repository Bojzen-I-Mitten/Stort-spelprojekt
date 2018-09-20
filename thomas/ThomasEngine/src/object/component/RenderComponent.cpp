#include "RenderComponent.h"
#pragma once
#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#pragma managed

#include "../Component.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"

namespace ThomasEngine
{
	RenderComponent::RenderComponent()
		: Component(new thomas::object::component::RenderComponent())
	{}

	Model^ RenderComponent::model::get()
	{
		return m_model;
	}

	void RenderComponent::model::set(Model^ value)
	{
		m_model = value;
		if (m_model == nullptr)
			((thomas::object::component::RenderComponent*)nativePtr)->SetModel(nullptr);
		else
			((thomas::object::component::RenderComponent*)nativePtr)->SetModel((thomas::resource::Model*)value->m_nativePtr);
	}



	Material^ RenderComponent::material::get() {
		thomas::resource::Material* nptr = ((thomas::object::component::RenderComponent*)nativePtr)->GetMaterial(0);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		else
			return gcnew Material(nptr);
	}
	void RenderComponent::material::set(Material^ value) {
		if (value)
			((thomas::object::component::RenderComponent*)nativePtr)->SetMaterial((thomas::resource::Material*)value->m_nativePtr);
		else
			((thomas::object::component::RenderComponent*)nativePtr)->SetMaterial(nullptr);

		OnPropertyChanged("material");
	}


	void RenderComponent::Update()
	{
		((thomas::object::component::RenderComponent*)nativePtr)->Update();
	}


}