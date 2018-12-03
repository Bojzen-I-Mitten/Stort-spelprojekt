#pragma unmanaged
#include <thomas\object\component\physics\Collider.h>
#pragma managed
#include "Collider.h"
#include "Rigidbody.h"
#include "../../GameObject.h"
#include "../../../ThomasManaged.h"
#include "../../../Scene.h"

namespace ThomasEngine
{
#define TCollider thomas::object::component::Collider
	Collider::Collider(TCollider * nativePtr) : Component(nativePtr)
	{
		OnCollisionDelegate^ fp = gcnew OnCollisionDelegate(this, &Collider::OnCollision);
		gch = GCHandle::Alloc(fp);
		IntPtr ip = Marshal::GetFunctionPointerForDelegate(fp);
		TCollider::OnCollisionEvent cb = static_cast<TCollider::OnCollisionEvent>(ip.ToPointer());
		nativePtr->SetOnCollisionEvent(cb);
	}
	Collider::~Collider() 
	{
		
	}
	void Collider::OnDestroy()
	{
		gch.Free();
	}

	void Collider::OnCollision(TCollider* otherCollider, COLLISION_TYPE collisionType)
	{
		GameObject^ obj = ThomasWrapper::CurrentScene->Find(otherCollider->m_gameObject);
		if (obj) // Verify object was found
			return;
		Collider^ collider = obj->GetComponent<Collider^>(otherCollider);
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
			((TCollider*)nativePtr)->SetAttachedRigidbody((thomas::object::component::Rigidbody*)value->nativePtr);
		}
		else
		{
			((TCollider*)nativePtr)->SetAttachedRigidbody(nullptr);
		}
	}

	bool Collider::isTrigger::get() { return ((TCollider*)nativePtr)->IsTrigger(); }
	void Collider::isTrigger::set(bool value) { ((TCollider*)nativePtr)->SetTrigger(value); }
}

