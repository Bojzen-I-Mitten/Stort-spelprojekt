#pragma once
#include "../Component.h"
namespace thomas { namespace object { namespace component { class Camera; } } }
namespace ThomasEngine
{
	[ExecuteInEditor]
	public ref class Camera : public Component
	{
	private:
		[Xml::Serialization::XmlIgnoreAttribute]
		property thomas::object::component::Camera* camera {
			thomas::object::component::Camera* get();
		}
	public:
		Camera();

		[BrowsableAttribute(false)]
		property Matrix viewMatrix {Matrix get();}

		[BrowsableAttribute(false)]
		property Matrix projectionMatrix{Matrix get();}

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
	};
}