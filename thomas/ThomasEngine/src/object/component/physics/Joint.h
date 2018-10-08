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
			Rigidbody^ get();
			void set(Rigidbody^ body);
			
		}

		property Vector3 Anchor
		{
			Vector3 get();
			void set(Vector3 origin);
		}

		property Vector3 ConnectedAnchor
		{
			Vector3 get();
			void set(Vector3 origin);
		}

		property Vector3 Axis
		{
			Vector3 get();
			void set(Vector3 rotation);
		}

		property Vector3 SwingAxis
		{
			Vector3 get();
			void set(Vector3 rotation);
		}

		property float TwistAngle
		{
			float get();
			void set(float value);
		}

		property float SwingAngle1
		{
			float get();
			void set(float value);
		}
		property float SwingAngle2
		{
			float get();
			void set(float value);
		}
		property bool NoCollision
		{
			bool get();
			void set(bool value);
		}

	};

}