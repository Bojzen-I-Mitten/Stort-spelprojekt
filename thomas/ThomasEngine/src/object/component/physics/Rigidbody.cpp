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

ThomasEngine::GameObject^ ThomasEngine::Rigidbody::GetTargetCollider()
{
	if (this != nullptr)
	{
		auto collider = ((thomas::object::component::Rigidbody*)nativePtr)->GetTargetCollider();

		if (collider != nullptr)
		{
			GameObject^ target = (GameObject^)Object::Find(Utility::Convert(collider->m_guid));
			((thomas::object::component::Rigidbody*)nativePtr)->ClearTargetCollider();
			return target;
		}
	}

	return nullptr;
}