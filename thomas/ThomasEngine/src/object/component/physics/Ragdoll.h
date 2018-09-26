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

		property Vector2 ChangeCapsule0
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(0)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 0); };
		}
		property Vector2 ChangeCapsule1
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(1)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 1); };
		}
		property Vector2 ChangeCapsule2
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(2)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 2); };
		}
		property Vector2 ChangeCapsule3
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(3)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 3); };
		}
		property Vector2 ChangeCapsule4
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(4)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 4); };
		}
		property Vector2 ChangeCapsule5
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(5)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 5); };
		}

		property Vector2 ChangeCapsule6
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(6)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 6); };
		}
		property Vector2 ChangeCapsule7
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(7)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 7); };
		}
		property Vector2 ChangeCapsule8
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(8)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 8); };
		}
		property Vector2 ChangeCapsule9
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Ragdoll*)nativePtr)->GetBoneCapsuls(9)); }
			void set(Vector2 value) { ((thomas::object::component::Ragdoll*)nativePtr)->SetBoneCapsuls(thomas::math::Vector2(value.x, value.y), 9); };
		}
	};
}