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
	private:
		Model^ m_model;
		property thomas::object::component::RenderComponent* render {thomas::object::component::RenderComponent* get(); }
	public:
		RenderComponent();
		property Model^ model {
			Model^ get();
			void set(Model^ value);
		}
		property Material^ material {
			Material^ get();
			void set(Material^ value);
		}
	};
}