#pragma once
#pragma unmanaged
#include <thomas\object\component\physics\Rigidbody.h>

#pragma managed
#include "../../Component.h"
#include "../../../Utility.h"
namespace ThomasEngine
{
	[DisallowMultipleComponent]
	public ref class Rigidbody : public Component
	{
	public:
		enum class ForceMode
		{
			Force,
			Impulse
		};

		enum class ActivationState
		{
			Default = WANTS_DEACTIVATION,
			Sleeping = ISLAND_SLEEPING,
			Disabled = DISABLE_SIMULATION,
			Active_Tag = ACTIVE_TAG,
			Always_Active = DISABLE_DEACTIVATION
		};

	public:
		Rigidbody() : Component(new thomas::object::component::Rigidbody()) {}
		void Awake() override;
		void OnDestroy() override;

		void AddTorque(Vector3 torque)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->AddTorque(thomas::math::Vector3(torque.x, torque.y, torque.z));
		}

		void AddTorque(Vector3 torque, ForceMode mode)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->AddTorque(thomas::math::Vector3(torque.x, torque.y, torque.z),
																(thomas::object::component::ForceMode)mode);
		}

		void AddForce(Vector3 force)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->AddForce(thomas::math::Vector3(force.x, force.y, force.z));
		}

		void AddForce(Vector3 force, ForceMode mode) 
		{ 
			((thomas::object::component::Rigidbody*)nativePtr)->AddForce(thomas::math::Vector3(force.x, force.y, force.z), 
																		   (thomas::object::component::ForceMode)mode); 
		}

		void AddRelativeForce(Vector3 force, Vector3 relPos)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->AddRelativeForce(thomas::math::Vector3(force.x, force.y, force.z),
																				   thomas::math::Vector3(relPos.x, relPos.y, relPos.z));
		}

		void AddRelativeForce(Vector3 force, Vector3 relPos, ForceMode mode)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->AddRelativeForce(thomas::math::Vector3(force.x, force.y, force.z), 
																thomas::math::Vector3(relPos.x, relPos.y, relPos.z), 
																(thomas::object::component::ForceMode)mode);
		}

		void SetActivationState(ActivationState state)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->SetActivationState((thomas::object::component::ActivationState)state);
		}


		property Vector3 LinearVelocity
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetLinearVelocity()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetLinearVelocity(Utility::Convert(value)); }
		}

		property Vector3 AngularVelocity
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetAngularVelocity()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetAngularVelocity(Utility::Convert(value)); }
		}

		property bool IsKinematic 
		{
			bool get() { return ((thomas::object::component::Rigidbody*)nativePtr)->IsKinematic(); }
			void set(bool value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetKinematic(value); }
		}

		property float Mass 
		{
			float get(){ return ((thomas::object::component::Rigidbody*)nativePtr)->GetMass(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetMass(value); }
		}

		property Vector3 FrzPos
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetFreezePosition()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetFreezePosition(ClampVec3(Utility::Convert(value), 0.f, 1.f)); }
		}

		property Vector3 FrzRot
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetFreezeRotation()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetFreezeRotation(ClampVec3(Utility::Convert(value), 0.f, 1.f)); }
		}

	private:
		thomas::math::Vector3 ClampVec3(thomas::math::Vector3 & value, const float & min, const float & max)
		{
			ClampFloat(value.x, min, max);
			ClampFloat(value.y, min, max);
			ClampFloat(value.z, min, max);
			return value;
		}

		void ClampFloat(float & value, const float & lo, const float & ho)
		{
			if (value < lo)
				value = lo;
			if (value > ho)
				value = ho;
		}
	};
}