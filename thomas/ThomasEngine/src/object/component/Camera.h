#pragma once

#pragma managed
#include "../Component.h"
#include "../../resource/texture/Texture2D.h"
#include "../../resource/Font.h"

namespace thomas { namespace object { namespace component { class Camera; } } }

namespace ThomasEngine
{
	[ExecuteInEditor]
	public ref class Camera : public Component
	{
	private:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::component::Camera* camera {
			thomas::object::component::Camera* get();
		}

	public:
		Camera();

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Matrix viewMatrix {Matrix get();}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Matrix projectionMatrix{Matrix get();}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 position{Vector3 get();}

		[DisplayNameAttribute("Field Of View")]
		property float fieldOfView
		{
			float get();
			void set(float value);
		}

		[DisplayNameAttribute("Near Plane")]
		property float nearPlane
		{
			float get();
			void set(float value);
		}

		[DisplayNameAttribute("Far Plane")]
		property float farPlane
		{
			float get();
			void set(float value);
		}

		property int targetDisplay
		{
			int get();
			void set(int value);
		}

		[DisplayNameAttribute("Render GUI")]
		property bool renderGUI
		{
			bool get();
			void set(bool value);
		}

	public:
		//GUI
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

		void AddText(String^ id, System::String^ text, Vector2 position);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector2 scale);
		void AddText(String^ id, System::String^ text, Vector2 position, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector4 color);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector2 scale);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector4 color);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector4 color, Vector2 scale);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector4 color, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector2 scale, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector4 color, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector4 color, Vector2 scale);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector2 scale, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Vector4 color, Vector2 scale, float rotation);
		void AddText(String^ id, System::String^ text, Vector2 position, Font^ font, Vector4 color, Vector2 scale, float rotation);

		void SetTextPosition(String^ id, Vector2 position);
		void SetTextColor(String^ id, Vector4 color);
		void SetTextScale(String^ id, Vector2 scale);
		void SetTextRotation(String^ id, float rotation);
	};
}