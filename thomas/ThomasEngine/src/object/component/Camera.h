#pragma once

#pragma managed
#include "../Component.h"
#include "../../resource/texture/Texture2D.h"
#include "../../resource/Font.h"

namespace thomas { namespace object { namespace component { class Camera; } } }

namespace ThomasEngine
{
	ref class Texture2D;
	[ExecuteInEditor]
	public ref class Camera : public Component
	{
	private:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::component::Camera* camera {
			thomas::object::component::Camera* get();
		}

	public:
		enum class FLIP_EFFECTS
		{
			NONE,
			FLIPHORIZONTALLY,
			FLIPVERTICALLY,
			FLIPBOTH
		};

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

		property Texture2D^ SkyMap
		{
			Texture2D^ get();
			void set(Texture2D^ value);
		}
		[BrowsableAttribute(false)]
		property Vector2 viewport
		{
			Vector2 get()
			{
				thomas::math::Viewport vp = ((thomas::object::component::Camera*)nativePtr)->GetViewport();
				return Vector2(vp.width, vp.height);
			}
		}

		/*[BrowsableAttribute(false)]
		property FLIP_EFFECTS flipEffects
		{
			FLIP_EFFECTS get();
			void set(FLIP_EFFECTS value);
		}*/

	public:

		//GUI Images
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
		void SetImageOrigin(String^ id, Vector2 origin);
		void SetImageFlipEffect(String^ id, FLIP_EFFECTS effect);
		void DeleteImage(String^ id);

		// GUI Text
		void AddText(String^ id, String^ text, Vector2 position);
		void AddText(String^ id, String^ text, Vector2 position, Vector2 scale);
		void AddText(String^ id, String^ text, Vector2 position, float rotation);
		void AddText(String^ id, String^ text, Vector2 position, Vector4 color);
		void AddText(String^ id, String^ text, Vector2 position, Font^ font);
		void AddText(String^ id, String^ text, Vector2 position, Vector2 scale, Font^ font);
		void AddText(String^ id, String^ text, Vector2 position, Font^ font, Vector4 color);
		void AddText(String^ id, String^ text, Vector2 position, Vector2 scale, float rotation, Vector4 color);
		void AddText(String^ id, String^ text, Vector2 position, Vector2 scale, Font^ font, Vector4 color);
		void AddText(String^ id, String^ text, Vector2 position, Vector2 scale, float rotation, Font^ font, Vector4 color);

		void SetText(String^ id, String^ newText);
		void SetTextPosition(String^ id, Vector2 position);
		void SetTextColor(String^ id, Vector4 color);
		void SetTextScale(String^ id, Vector2 scale);
		void SetTextRotation(String^ id, float rotation);
		void SetTextFont(String^ id, Font^ font);
		void SetTextOrigin(String^ id, Vector2 origin);
		void SetTextFlipEffect(String^ id, FLIP_EFFECTS effect);
		Vector2 GetTextSize(String^ id);
		void DeleteText(String^ id);
	};
}