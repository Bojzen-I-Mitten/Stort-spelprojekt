#pragma once
#pragma unmanaged
#include <thomas\object\component\physics\Rigidbody.h>

#pragma managed
#include "../../Component.h"
#include "../../GameObject.h"

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

		GameObject^ GetTargetCollider()
		{
			if (this != nullptr)
			{
				auto collider = ((thomas::object::component::Rigidbody*)nativePtr)->GetTargetCollider();

				if (collider != nullptr)
				{
					String^ name = gcnew String(collider->GetName().c_str());
					return GameObject::Find(name);
				}
			}

			return nullptr;
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

		property bool UseGravity
		{
			bool get() { return ((thomas::object::component::Rigidbody*)nativePtr)->HasGravity(); }
			void set(bool value) { ((thomas::object::component::Rigidbody*)nativePtr)->SetGravity(value); }
		}
	};
}