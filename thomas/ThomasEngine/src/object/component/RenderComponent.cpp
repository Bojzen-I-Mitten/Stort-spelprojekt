#pragma unmanaged
#include <thomas\object\component\RenderComponent.h>
#pragma managed
#include "RenderComponent.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../resource/Resources.h"
#include "../../Debug.h"

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

		OnPropertyChanged("materials");
	}


	array<Material^>^ RenderComponent::materials::get()
	{
		auto nativeMats = render->GetMaterials();
		array<Material^>^ mats = gcnew array<Material^>((int)nativeMats.size());
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
				if (value[i] == nullptr)
					value[i] = Material::StandardMaterial;
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

	Material ^ RenderComponent::FindMaterial(String ^ name)
	{
		uint32_t hash = Utility::hash(name);
		thomas::resource::Material* nptr = render->findMaterial(hash);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		return nullptr;
	}

	int RenderComponent::FindMaterialIndex(String ^ name)
	{
		uint32_t hash = Utility::hash(name);
		return render->findMaterialIndex(hash);
	}

	Material ^ RenderComponent::CreateMaterialInstance(String ^ name)
	{
		int index = FindMaterialIndex(name);
		Material^ old = GetMaterial(index);
		if (old)
		{
			Material^ cpy = gcnew Material(old);
			SetMaterial(index, cpy);
			return cpy;
		}
		return nullptr;
	}
	Material^ RenderComponent::GetMaterial(int index)
	{
		if (index < 0 || render->numMeshes() <= index) return nullptr;
		thomas::resource::Material* nptr = render->GetMaterial(index);
		Resource^ mat = ThomasEngine::Resources::FindResourceFromNativePtr(nptr);
		if (mat != nullptr)
			return (Material^)mat;
		return nullptr;
	}
	void RenderComponent::SetMaterial(int index, Material^ mat)
	{
		if(mat == nullptr)
			mat = Material::StandardMaterial;
		if (index < 0 || render->numMeshes() <= index)
		{
			Debug::LogWarning("Failed to set material " + mat->Name + " at index: " + index + ", in RenderComponent: " + Name);
			return;
		}
		render->SetMaterial(index, mat->Native);
	}

	bool RenderComponent::EnableCulling::get()
	{
		return render->EnabledCulling();
	}

	void RenderComponent::EnableCulling::set(bool value)
	{
		render->EnableCulling(value);
	}
}