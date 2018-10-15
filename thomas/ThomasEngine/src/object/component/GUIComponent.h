#pragma once

#pragma managed
#include "../Component.h"
#include "../../Utility.h"
#include "../../resource/texture/Texture2D.h"

namespace thomas { namespace object { namespace component { class GUIComponent; } } }

namespace ThomasEngine
{
	[DisallowMultipleComponent]
	public ref class GUIComponent : public Component
	{
	private:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::component::GUIComponent* handle 
		{
			thomas::object::component::GUIComponent* get();
		}

	public:
		GUIComponent();

		void AddImage(String^ id, Texture2D^ texture, Vector2 position, bool interact);
		void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector4 color, bool interact);
		void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, bool interact);
		void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, bool interact);
		void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, Vector4 color, bool interact);

		bool OnImageClicked(String^ id);
		bool OnImageHovered(String^ id);

		void SetImageTexture(String^ id, Texture2D^ texture);
		void SetImagePosition(String^ id, Vector2 position);
		void SetImageColor(String^ id, Vector4 color);
		void SetImageScale(String^ id, Vector2 scale);
		void SetImageRotation(String^ id, float rotation);
		void SetImageInteract(String^ id, bool interact);
	};
}