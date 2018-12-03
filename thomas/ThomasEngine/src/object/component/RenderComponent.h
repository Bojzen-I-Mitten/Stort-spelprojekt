#pragma once
#include "../Component.h"

#pragma managed


namespace thomas { namespace object { namespace component { class RenderComponent; } } }
namespace ThomasEngine
{
	ref class Model;
	ref class Material;
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	public ref class RenderComponent : public Component
	{
	protected:
		Model^ m_model;
		property thomas::object::component::RenderComponent* render {thomas::object::component::RenderComponent* get(); }
	public:
		RenderComponent();
		RenderComponent(thomas::object::component::RenderComponent* inherit);

		virtual void OnAwake() override {}

		property Model^ model {
			Model^ get();
			void set(Model^ value);
		}

		property array<Material^>^ materials
		{
			array<Material^>^ get();
			void set(array<Material^>^ value);
		}

		Material^ GetMaterial(int index);
		void SetMaterial(int index, Material ^ mat);
		/* Get material by name. Null if nothing found.
		*/
		Material^ FindMaterial(String^ name);
		/* Find the index of a specific material. Can be used to fetch the associated mesh.
		*/
		int FindMaterialIndex(String^ name);
		/* Fetch a material by name and create a local copy from it and apply it to the same slot.
		name	<<	Name of the material to copy, only the first material with the name is copied.
		return	>>	Local copy of the material. Null if no name matched. 
		*/
		Material^ CreateMaterialInstance(String^ name);

		[BrowsableAttribute(false)]
		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}
	};
}