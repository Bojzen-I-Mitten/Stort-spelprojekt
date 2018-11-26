#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#pragma managed
#include "RenderComponent.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../resource/Resources.h"

namespace ThomasEngine
{
	RenderComponent::RenderComponent()
		: Component(new thomas::object::component::RenderComponent())
	{}

	RenderComponent::RenderComponent(thomas::object::component::RenderComponent* inherit) : Component(inherit)
	{}

	thomas::object::component::RenderComponent* RenderComponent::render::get() { return (thomas::object::component::RenderComponent*)nativePtr; }

	Model^ RenderComponent::model::get() { return m_model; }

	void RenderComponent::model::set(Model^ value) {
		m_model = value;
		if (m_model == nullptr)
			render->SetModel(nullptr);
		else
			render->SetModel((thomas::resource::Model*)value->m_nativePtr);
	}


	array<Material^>^ RenderComponent::materials::get()
	{
		auto nativeMats = render->GetMaterials();
		array<Material^>^ mats = gcnew array<Material^>(nativeMats.size());
		for (int i=0; i < mats->Length; i++)
		{
			Resource^ resource = ThomasEngine::Resources::FindResourceFromNativePtr(nativeMats[i]);
			if (resource == nullptr)
				resource = Material::StandardMaterial;

			mats[i] = (Material^)resource;
		}
		return mats;
	}

	void RenderComponent::materials::set(array<Material^>^ value)
	{
		if (value != nullptr) {
			std::vector<thomas::resource::Material*> nativeMats(value->Length);
			for (int i = 0; i < value->Length; i++)
			{
				nativeMats[i] = (thomas::resource::Material*)value[i]->m_nativePtr;
			}
			render->SetMaterials(nativeMats);
		}

	}


	Material^ RenderComponent::material::get()
	{
		thomas::resource::Material* nptr = render->GetMaterial(0);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		else
			return Material::StandardMaterial; // Prevent new (empty) materials to be generated during deserialization.
			//return gcnew Material(nptr);
	}

	void RenderComponent::material::set(Material^ value)
	{
		if (value)
			render->SetMaterial(0, (thomas::resource::Material*)value->m_nativePtr);
		else
			render->SetMaterial(0, nullptr);

		OnPropertyChanged("material");
	}



}