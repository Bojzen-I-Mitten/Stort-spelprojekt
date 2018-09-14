#pragma once
#pragma unmanaged
#include <thomas\object\component\Camera.h>
#pragma managed
#include "../../Utility.h"
#include "../Component.h"
#include <string>
#include <msclr\marshal_cppstd.h>

namespace ThomasEngine
{
	public ref class Camera : public Component
	{
	public:
		Camera() : Component(new thomas::object::component::Camera()) {};

		[BrowsableAttribute(false)]
		property Matrix viewMatrix
		{
			Matrix get() { return Utility::Convert(((thomas::object::component::Camera*)nativePtr)->GetViewMatrix()); }
		}

		[BrowsableAttribute(false)]
		property Matrix projectionMatrix
		{
			Matrix get() { return Utility::Convert(((thomas::object::component::Camera*)nativePtr)->GetProjMatrix()); }
		}

		[BrowsableAttribute(false)]
		property Vector3 position
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Camera*)nativePtr)->GetPosition()); }
		}

		[DisplayNameAttribute("Field Of View")]
		property float fieldOfView
		{
			float get() { return ((thomas::object::component::Camera*)nativePtr)->GetFov(); }
			void set(float value) { ((thomas::object::component::Camera*)nativePtr)->SetFov(value); }
		}

		[DisplayNameAttribute("Near Plane")]
		property float nearPlane
		{
			float get() { return ((thomas::object::component::Camera*)nativePtr)->GetNear(); }
			void set(float value) { ((thomas::object::component::Camera*)nativePtr)->SetNear(value); }
		}

		[DisplayNameAttribute("Far Plane")]
		property float farPlane
		{
			float get() { return ((thomas::object::component::Camera*)nativePtr)->GetFar(); }
			void set(float value) { ((thomas::object::component::Camera*)nativePtr)->SetFar(value); }
		}

		property int targetDisplay
		{
			int get() { return ((thomas::object::component::Camera*)nativePtr)->GetTargetDisplayIndex(); }
			void set(int value) { ((thomas::object::component::Camera*)nativePtr)->SetTargetDisplay(value); }
		}
	};
}