#pragma once
#pragma unmanaged
#include <thomas/object/component/physics/Joint.h>

#pragma managed
#include "../../component/physics/Rigidbody.h"
#include "../../Component.h"
#include "../../../Utility.h"

namespace ThomasEngine
{
	[DisallowMultipleComponent]
	public ref class Joint : public Component
	{
	public:
		Joint() : Component(new thomas::object::component::Joint()) {}
	private:
		Rigidbody^ m_attachedRigidbody;
	public:
		property Rigidbody^ AttachedRigidbody
		{
			Rigidbody^ get() { return m_attachedRigidbody; }
			void set(Rigidbody^ body) 
			{
				m_attachedRigidbody = body;
				if (body)
					((thomas::object::component::Joint*)nativePtr)->SetRigidBody((thomas::object::component::Rigidbody*)body->nativePtr);
				else
					((thomas::object::component::Joint*)nativePtr)->SetRigidBody(nullptr);
			}
		}

		property Vector3 OriginCurrent
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetTransformOrigin(true)); }
			void set(Vector3 origin) { ((thomas::object::component::Joint*)nativePtr)->SetTransformOrigin(Utility::Convert(origin), true); }
		}

		property Vector3 OriginOther
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetTransformOrigin(false)); }
			void set(Vector3 origin) { ((thomas::object::component::Joint*)nativePtr)->SetTransformOrigin(Utility::Convert(origin), false); }
		}

		property Vector3 RotationCurrent
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetTransformRotation(true)); }
			void set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetTransformRotation(Utility::Convert(rotation), true); }
		}

		property Vector3 RotationOther
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetTransformRotation(false)); }
			void set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetTransformRotation(Utility::Convert(rotation), false); }
		}

		property Vector3 TwistSpin
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetTransformTwistSpin()); }
			void set(Vector3 twistSpin) { ((thomas::object::component::Joint*)nativePtr)->SetTransformTwistSpin(Utility::Convert(twistSpin)); }
		}
	};

}