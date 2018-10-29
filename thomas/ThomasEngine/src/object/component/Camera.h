#pragma once
#pragma unmanaged
#pragma managed
#include "../Component.h"
namespace thomas { namespace object { namespace component { class Camera; } } }

namespace ThomasEngine
{
	ref class Viewport;
	ref class Canvas;
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

		[BrowsableAttribute(false)]
		property Viewport viewport
		{
			Viewport get();
		}

	public:
		Canvas^ AddCanvas();
		Canvas^ AddCanvas(Viewport viewport);
	};
}