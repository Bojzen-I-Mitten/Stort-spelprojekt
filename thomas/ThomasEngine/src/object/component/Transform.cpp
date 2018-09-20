#include "Transform.h"
#include "../GameObject.h"
#include "../../ThomasManaged.h"
void ThomasEngine::Transform::SetParent(Transform ^ value)
{
	SetParent(value, true);
}
void ThomasEngine::Transform::SetParent(Transform ^ value, bool worldPositionStays)
{
	if (value)
		((thomas::object::component::Transform*)nativePtr)->SetParent((thomas::object::component::Transform*)value->nativePtr, worldPositionStays);
	else
		((thomas::object::component::Transform*)nativePtr)->SetParent(nullptr, worldPositionStays);
}
void ThomasEngine::Transform::OnDestroy()
{
	if (parent)
	{
		parent = nullptr;
	}
	for (int i = 0; i < children->Count; i++)
	{
		children[i]->gameObject->Destroy();
		//m_children.erase(m_children.begin() + i);
		i -= 1;
	}
}

void ThomasEngine::Transform::parent::set(ThomasEngine::Transform^ value)
{
	Transform^ oldParent = parent;
	SetParent(value);
	OnParentChanged(this, oldParent, value);
}