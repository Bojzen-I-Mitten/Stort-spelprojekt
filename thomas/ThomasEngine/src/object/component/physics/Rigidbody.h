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

		property ActivationState ActiveState
		{
			ActivationState get() { return (ActivationState)((thomas::object::component::Rigidbody*)nativePtr)->GetActivationState(); };
			void set(ActivationState state) { ((thomas::object::component::Rigidbody*)nativePtr)->SetActivationState((thomas::object::component::ActivationState)state); }
		}
		[BrowsableAttribute(false)]
		[CategoryAttribute("Velocity")]
		property Vector3 LinearVelocity
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetLinearVelocity()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetLinearVelocity(Utility::Convert(value)); }
		}
		[BrowsableAttribute(false)]
		[CategoryAttribute("Velocity")]
		property Vector3 AngularVelocity
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetAngularVelocity()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetAngularVelocity(Utility::Convert(value)); }
		}
		[DisplayNameAttribute("Is Kinematic")]
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

		property float Friction
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetFriction(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetFriction(value); }
		}

		property float Bounciness
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetBounciness(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetBounciness(value); }
		}
		[DisplayNameAttribute("Freeze Position")]
		[CategoryAttribute("Constraints")]
		property Vector3 FreezePosition
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetFreezePosition()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetFreezePosition(ClampVec3(Utility::Convert(value), 0.f, 1.f)); }
		}
		[DisplayNameAttribute("Freeze Rotation")]
		[CategoryAttribute("Constraints")]
		property Vector3 FreezeRotation
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetFreezeRotation()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetFreezeRotation(ClampVec3(Utility::Convert(value), 0.f, 1.f)); }
		}

		[BrowsableAttribute(false)]
		property Vector3 CenterOfMass
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetCenterOfmass()); }
			void set(Vector3 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetCenterOfmass(Utility::Convert(value)); }
		}


		property float Damping
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetDamping(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetDamping(value); }
		}
		[DisplayNameAttribute("Angular Damping")]
		property float AngularDamping
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetDamping(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetDamping(value); }
		}

		[BrowsableAttribute(false)]
		[CategoryAttribute("Thresholds")]
		property Vector2 SleepingThresholds
		{
			Vector2 get() { return Utility::Convert(((thomas::object::component::Rigidbody*)nativePtr)->GetSleepingThresholds()); }
			void set(Vector2 value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetSleepingThresholds(Utility::Convert(value)); }
		}
		
		[BrowsableAttribute(false)]
		[CategoryAttribute("Thresholds")]
		property float DeactivationTime
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetDeactivationTime(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetDeactivationTime(value); }
		}
		[BrowsableAttribute(false)]
		[CategoryAttribute("Thresholds")]
		property float ContactProcessingThresholds
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetContactProcessingThreshold(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetContactProcessingThreshold(value); }
		}
		[BrowsableAttribute(false)]
		[CategoryAttribute("Thresholds")]
		property float CcdMotionThreshold
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetCcdMotionThreshold(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetCcdMotionThreshold(value); }
		}
		[BrowsableAttribute(false)]
		[CategoryAttribute("Thresholds")]
		property float CcdSweptSphereRadius
		{
			float get() { return ((thomas::object::component::Rigidbody*)nativePtr)->GetCcdSweptSphereRadius(); }
			void set(float value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetCcdSweptSphereRadius(value); }
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