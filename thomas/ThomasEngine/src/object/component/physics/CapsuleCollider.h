#pragma once
#pragma unmanaged
#include <thomas\object\component\physics\CapsuleCollider.h>
#pragma managed
#include "Collider.h"
#include "../../../Utility.h"
namespace ThomasEngine
{
	public ref class CapsuleCollider : public Collider
	{
	public:
		enum class ColliderRotation
		{
			RotateX,
			RotateY,
			RotateZ
		};

	public:
		CapsuleCollider() : Collider(new thomas::object::component::CapsuleCollider()) {}

		property Vector3 center
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::CapsuleCollider*)nativePtr)->getCenter()); }
			void set(Vector3 value) { ((thomas::object::component::CapsuleCollider*)nativePtr)->SetCenter(thomas::math::Vector3(value.x, value.y, value.z)); }
		}

		property float radius
		{
			float get() { return ((thomas::object::component::CapsuleCollider*)nativePtr)->GetRadius(); }
			void set(float value) { ((thomas::object::component::CapsuleCollider*)nativePtr)->SetRadius(value); OnPropertyChanged("height"); }
		}

		property float height
		{
			float get() { return ((thomas::object::component::CapsuleCollider*)nativePtr)->GetHeight(); }
			void set(float value) { ((thomas::object::component::CapsuleCollider*)nativePtr)->SetHeight(value); }
		}

		property ColliderRotation rotation
		{
			ColliderRotation get() { return (ColliderRotation)((thomas::object::component::CapsuleCollider*)nativePtr)->GetRotation(); }
			void set(ColliderRotation value) { ((thomas::object::component::CapsuleCollider*)nativePtr)->SetRotation((thomas::object::component::ColliderRotation)value); }
		}
	};
}