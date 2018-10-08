#include "Joint.h"

namespace ThomasEngine
{
	//Rigidbody
	Rigidbody^ Joint::ConnectedRigidbody::get() { return m_connectedRigidbody; }
	void Joint::ConnectedRigidbody::set(Rigidbody^ body) 
	{
		m_connectedRigidbody = body;
		if (body)
			((thomas::object::component::Joint*)nativePtr)->SetConnectedBody((thomas::object::component::Rigidbody*)body->nativePtr);
		else
			((thomas::object::component::Joint*)nativePtr)->SetConnectedBody(nullptr);
	}
	//Anchor
	Vector3 Joint::Anchor::get(){ return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameAAnchor()); }
	void Joint::Anchor::set(Vector3 origin){ ((thomas::object::component::Joint*)nativePtr)->SetFrameAAnchor(Utility::Convert(origin)); }

	//ConnectedAnchor
	Vector3 Joint::ConnectedAnchor::get(){ return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameBAnchor()); }
	void Joint::ConnectedAnchor::set(Vector3 origin){ ((thomas::object::component::Joint*)nativePtr)->SetFrameBAnchor(Utility::Convert(origin)); }

	//Axis
	Vector3 Joint::Axis::get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameAAxis()); }
	void Joint::Axis::set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetFrameAAxis(Utility::Convert(rotation)); }

	//SwingAxis
	Vector3 Joint::SwingAxis::get() { return Utility::Convert(((thomas::object::component::Joint*)nativePtr)->GetFrameBAxis()); }
	void Joint::SwingAxis::set(Vector3 rotation) { ((thomas::object::component::Joint*)nativePtr)->SetFrameBAxis(Utility::Convert(rotation)); }

	//SwingAngle
	float Joint::SwingAngle::get() { return ((thomas::object::component::Joint*)nativePtr)->GetTwist(); }
	void Joint::SwingAngle::set(float value) { return ((thomas::object::component::Joint*)nativePtr)->SetTwist(value); }

	//twistAngle
	float Joint::TwistAngle::get() { return ((thomas::object::component::Joint*)nativePtr)->GetSwing1(); }
	void Joint::TwistAngle::set(float value) { return ((thomas::object::component::Joint*)nativePtr)->SetSwing1(value); }
}