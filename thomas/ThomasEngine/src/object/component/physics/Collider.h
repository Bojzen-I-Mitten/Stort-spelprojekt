#pragma once
#include "../../Component.h"
using namespace System::Runtime::InteropServices;
using namespace thomas::object;
namespace ThomasEngine
{
	ref class Rigidbody;
	[HideInInspector]
	[DisallowMultipleComponent]
	public ref class Collider : public Component
	{
	internal:
		Collider() : Component(nullptr){}
		enum class COLLISION_TYPE
		{
			STARTED,
			STAY,
			ENDED
		};

		virtual void OnGameObjectSet() override {};
	private:
		Rigidbody^ m_attachedRigidbody;
		delegate void OnCollisionDelegate(component::Collider* otherCollider, COLLISION_TYPE collisionType);
		GCHandle gch;
		void OnCollision(component::Collider* otherCollider, COLLISION_TYPE collisionType);
	public:
		Collider(component::Collider* nativePtr);
		~Collider();
		

		[BrowsableAttribute(false)]
		property Rigidbody^ attachedRigidbody {
			Rigidbody^ get() { return m_attachedRigidbody; }
			void set(Rigidbody^ value);
			
		}


		property bool isTrigger
		{
			bool get();
			void set(bool value);
		}
	};
}