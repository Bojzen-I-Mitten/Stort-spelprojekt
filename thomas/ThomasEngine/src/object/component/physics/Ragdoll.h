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
		// Body Head
		property Vector3 BodyHeadtwist
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetTwistSpin(0)); };
			void set(Vector3 value) {((thomas::object::component::Ragdoll*)nativePtr)->SetTwistSpin(thomas::math::Vector3(value.x, value.y, value.z),0); };
		}
		property Vector3 BodyHeadFirst
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(0,false)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 0,false); };
		}
		property Vector3 BodyHeadSecond
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(0, true)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 0, true); };
		}
		// Left Arm
		property Vector3 Bodyleftarmtwist
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetTwistSpin(1)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetTwistSpin(thomas::math::Vector3(value.x, value.y, value.z), 1); };
		}
		property Vector3 BodyleftarmFirst
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(1, false)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 1, false); };
		}
		property Vector3 BodyleftarmSecond
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(1, true)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 1, true); };
		}

		// left upper and left lower arm
		property Vector3 leftarmlowerarmtwist
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetTwistSpin(1)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetTwistSpin(thomas::math::Vector3(value.x, value.y, value.z), 1); };
		}
		property Vector3 leftarmlowerarmFirst
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(1, false)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 1, false); };
		}
		property Vector3 leftarmlowerarmSecond
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->getFirstorSecond(1, true)); };
			void set(Vector3 value) { ((thomas::object::component::Ragdoll*)nativePtr)->setFirstorSecond(thomas::math::Vector3(value.x, value.y, value.z), 1, true); };
		}



		

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