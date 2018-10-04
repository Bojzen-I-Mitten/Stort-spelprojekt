#pragma unmanaged
#include <thomas\object\component\physics\Collider.h>
#pragma managed
#include "Collider.h"
#include "Rigidbody.h"
#include "../../GameObject.h"

namespace ThomasEngine
{

	Collider::Collider(component::Collider * nativePtr) : Component(nativePtr)
	{
		OnCollisionDelegate^ fp = gcnew OnCollisionDelegate(this, &Collider::OnCollision);
		gch = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		component::Collider::OnCollisionEvent cb = static_cast<component::Collider::OnCollisionEvent>(ip.ToPointer());
		nativePtr->SetOnCollisionEvent(cb);
	}
	Collider::~Collider() 
	{
		
	}

	void Collider::OnDestroy()
	{
		gch.Free();
	}

	void Collider::OnCollision(component::Collider* otherCollider, COLLISION_TYPE collisionType)
	{
		Collider^ collider = (Collider^)Object::Find(Utility::Convert(otherCollider->m_guid));
		if (!collider)
			return;
		if (collider->isTrigger || isTrigger) //If one of the colliders is a trigger we do OnColliderX instead.
		{
			switch (collisionType)
			{
			case COLLISION_TYPE::STARTED:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnTriggerEnter(collider);
				}
				break;
			case COLLISION_TYPE::STAY:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnTriggerStay(collider);
				}
				break;
			case COLLISION_TYPE::ENDED:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnTriggerExit(collider);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			switch (collisionType)
			{
			case COLLISION_TYPE::STARTED:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnCollisionEnter(collider);
				}
				break;
			case COLLISION_TYPE::STAY:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnCollisionStay(collider);
				}
				break;
			case COLLISION_TYPE::ENDED:
				for (int i = 0; i < gameObject->Components->Count; i++)
				{
					gameObject->Components[i]->OnCollisionExit(collider);
				}
				break;
			default:
				break;
			}
		}
		
	}

	void Collider::attachedRigidbody::set(Rigidbody^ value)
	{
		m_attachedRigidbody = value;
		if (m_attachedRigidbody)
		{
			((component::Collider*)nativePtr)->SetAttachedRigidbody((component::Rigidbody*)value->nativePtr);
		}
		else
		{
			((component::Collider*)nativePtr)->SetAttachedRigidbody(nullptr);
		}
	}

	bool Collider::isTrigger::get() { return ((component::Collider*)nativePtr)->IsTrigger(); }
	void Collider::isTrigger::set(bool value) { ((component::Collider*)nativePtr)->SetTrigger(value); }
}

