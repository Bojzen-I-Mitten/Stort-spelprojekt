#pragma unmanaged
#include <thomas\object\GameObject.h>

#pragma managed
#include "Rigidbody.h"
#include "Collider.h"
#include "../../GameObject.h"

void ThomasEngine::Rigidbody::Awake()
{
	Collider^ collider = gameObject->GetComponent<Collider^>();
	if (collider)
	{
		collider->attachedRigidbody = this;
		((thomas::object::component::Rigidbody*)nativePtr)->SetCollider((thomas::object::component::Collider*)collider->nativePtr);
	}
}

void ThomasEngine::Rigidbody::OnDestroy()
{
	List<Collider^>^ colliders = gameObject->GetComponents<Collider^>();
	for each(Collider^ c in colliders)
	{
		c->attachedRigidbody = nullptr;
	}
	Component::OnDestroy();
}