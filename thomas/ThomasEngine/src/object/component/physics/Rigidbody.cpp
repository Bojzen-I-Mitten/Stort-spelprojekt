#pragma unmanaged
#include <thomas\object\GameObject.h>
#pragma managed


#include "Rigidbody.h"
#include "Collider.h"
#include "../../GameObject.h"

namespace ThomasEngine
{
	void Rigidbody::Awake()
	{
		Collider^ collider = gameObject->GetComponent<Collider^>();
		if (collider)
		{
			collider->attachedRigidbody = this;
			AttachedCollider = collider;
			((thomas::object::component::Rigidbody*)nativePtr)->SetCollider((thomas::object::component::Collider*)collider->nativePtr);
		}
	}

	void Rigidbody::OnDestroy()
	{
		List<Collider^>^ colliders = gameObject->GetComponents<Collider^>();
		if (AttachedCollider)
		{
			AttachedCollider->attachedRigidbody = nullptr;
			AttachedCollider = nullptr;
		}
		Component::OnDestroy();
	}

}

