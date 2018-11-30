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

		[BrowsableAttribute(false)]
		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}
	};
}