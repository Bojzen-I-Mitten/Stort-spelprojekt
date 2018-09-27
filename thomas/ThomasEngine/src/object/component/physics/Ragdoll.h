#pragma once
#pragma managed
#include "../../Component.h"
#include "../../../Utility.h"
namespace ThomasEngine
{
	[DisallowMultipleComponent]
	public ref class Ragdoll : public Component
	{
	internal:
	public:
		Ragdoll();
	//	void Update() override
/*
		property Vector3 ChangeCapsule0
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(0)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 0); };
		}
		property Vector3 ChangeCapsule1
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(1)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 1); };
		}
		property Vector3 ChangeCapsule2
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(2)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 2); };
		}
		property Vector3 ChangeCapsule3
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(3)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 3); };
		}
		property Vector3 ChangeCapsule4
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(4)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 4); };
		}
		property Vector3 ChangeCapsule5
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(5)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 5); };
		}

		property Vector3 ChangeCapsule6
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(6)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 6); };
		}
		property Vector3 ChangeCapsule7
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(7)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 7); };
		}
		property Vector3 ChangeCapsule8
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(8)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 8); };
		}
		property Vector3 ChangeCapsule9
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(9)); }
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector3(value.x, value.y, value.z), 9); };
		}
		*/
	};
}