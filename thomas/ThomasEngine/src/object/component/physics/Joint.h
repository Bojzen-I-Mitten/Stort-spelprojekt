#pragma once
#pragma unmanaged
#include <thomas/object/component/physics/Joint.h>

#pragma managed
#include "../../component/physics/Rigidbody.h"
#include "../../Component.h"
#include "../../../Utility.h"

namespace ThomasEngine
{
	//[DisallowMultipleComponent]
	public ref class Joint : public Component
	{
	public:
		Joint() : Component(new thomas::object::component::Joint()) {}
	private:
		Rigidbody^ m_connectedRigidbody;
	public:
		property Rigidbody^ ConnectedRigidbody
		{
			Rigidbody^ get() { return m_connectedRigidbody; }
			void set(Rigidbody^ body) 
			{
				m_connectedRigidbody = body;
				if (body)
					((thomas::object::component::Joint*)nativePtr)->SetConnectedBody((thomas::object::component::Rigidbody*)body->nativePtr);
				else
					((thomas::object::component::Joint*)nativePtr)->SetConnectedBody(nullptr);
			}
		}

		property Vector3 Anchor
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameAAnchor()); }
			void set(Vector3 origin) { ((thomas::object::component::Joint*)nativePtr)->SetFrameAAnchor(Utility::Convert(origin)); }
		}

		property Vector3 ConnectedAnchor
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameBAnchor()); }
			void set(Vector3 origin) { ((thomas::object::component::Joint*)nativePtr)->SetFrameBAnchor(Utility::Convert(origin)); }
		}

		property Vector3 Axis
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameAAxis()); }
			void set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetFrameAAxis(Utility::Convert(rotation)); }
		}

		property Vector3 SwingAxis
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameBAxis()); }
			void set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetFrameBAxis(Utility::Convert(rotation)); }
		}

		property float TwistAngle
		{
			float get(){ return ((thomas::object::component::Joint*)nativePtr)->GetTwist();}
			void set(float value) { return ((thomas::object::component::Joint*)nativePtr)->SetTwist(value); }
		}

		property float SwingAngle
		{
			float get() { return ((thomas::object::component::Joint*)nativePtr)->GetSwing(); }
			void set(float value) { return ((thomas::object::component::Joint*)nativePtr)->SetSwing(value); }
		}
	};

}