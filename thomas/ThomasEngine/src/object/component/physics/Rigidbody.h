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
		Rigidbody() : Component(new thomas::object::component::Rigidbody()) {}
		

		void Awake() override;

		void OnDestroy() override;

		void ApplyCentralForce(Vector3 force) 
		{ 
			((thomas::object::component::Rigidbody*)nativePtr)->ApplyCentralForce(thomas::math::Vector3(force.x, force.y, force.z)); 
		}

		void ApplyForce(Vector3 force, Vector3 relPos)
		{
			((thomas::object::component::Rigidbody*)nativePtr)->ApplyForce(
			thomas::math::Vector3(force.x, force.y, force.z),
			thomas::math::Vector3(relPos.x, relPos.y, relPos.z));
		}

		bool HasCollided(GameObject^ collider)
		{
			msclr::interop::marshal_context context;
			return ((thomas::object::component::Rigidbody*)nativePtr)->
					HasCollided(context.marshal_as<thomas::object::GameObject*>(collider));
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
	};
}