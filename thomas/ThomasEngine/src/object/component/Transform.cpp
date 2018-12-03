#pragma unmanaged
#include <thomas\object\component\Transform.h>
#pragma managed
#include "Transform.h"
#include "../GameObject.h"
#include "../../Utility.h"
#include "../../ThomasManaged.h"

namespace ThomasEngine
{
	Transform::Transform() 
		: Component(new thomas::object::component::Transform()),
		m_children(gcnew List<Transform^>())
	{}

	thomas::object::component::Transform* Transform::trans::get() { return (thomas::object::component::Transform*)nativePtr; }



	Transform^ Transform::parent::get()
	{
		return m_parent;
	}
	void Transform::AddChild(Transform^ child)
	{
		m_children->Add(child);
	}

	bool Transform::RemoveChild(Transform^ child)
	{
		return m_children->Remove(child);
	}
	Transform ^ Transform::SwapParent(Transform ^ new_parent, bool worldPositionStays)
	{
		// Swap
		Transform^ oldParent = m_parent;
		m_parent = new_parent;
		// Add to new parent / Swap native
		if (new_parent)
		{
			new_parent->AddChild(this);
			((thomas::object::component::Transform*)nativePtr)->SetParent(new_parent->Native, worldPositionStays);
		}
		else
			((thomas::object::component::Transform*)nativePtr)->SetParent(nullptr, worldPositionStays);
		return oldParent;
	}
	/* Called when parent is destroyed rather then swapped out
	*/
	void Transform::OnParentDestroy(Transform ^ new_parent)
	{
		Transform^ oldParent = SwapParent(new_parent, true);
		OnParentChanged(this, oldParent, new_parent);
	}
	void Transform::OnParentDestroy(GameObject^ parented_object)
	{
		// Loop children
		for each (Transform^ t in children)
		{
			for each(Component^ c in t->m_gameObject->Components)
				c->OnParentDestroy(parented_object);
		}
	}
	void Transform::SetParent(Transform ^ new_parent, bool worldPositionStays)
	{
		if (new_parent == m_parent)
			return;	// Do nothing
		// Swap and clear
		Transform^  oldParent = SwapParent(new_parent, worldPositionStays);
		if (oldParent) {
			bool result = oldParent->RemoveChild(this);
			assert(result);
		}
			
		// Trigger event
		OnParentChanged(this, oldParent, new_parent);
	}
	void Transform::SetParent(Transform ^ value)
	{
		SetParent(value, true);
	}
	void Transform::parent::set(ThomasEngine::Transform^ value)
	{
		SetParent(value, false);
	}

	IEnumerable<Transform^>^ Transform::children::get()
	{
		return m_children;
	}

	Matrix Transform::world::get(){ return Utility::Convert(trans->GetWorldMatrix());}
	void Transform::world::set(Matrix value)
	{
		trans->SetWorldMatrix(Utility::Convert(value));
		OnPropertyChanged("localPosition");
		OnPropertyChanged("localEulerAngles");
		OnPropertyChanged("localScale");
	}
	Matrix Transform::local_world::get() { return Utility::Convert(trans->GetLocalWorldMatrix()); }
	void Transform::local_world::set(Matrix value)
	{
		trans->SetLocalMatrix(Utility::Convert(value));
		OnPropertyChanged("localPosition");
		OnPropertyChanged("localEulerAngles");
		OnPropertyChanged("localScale");
	}


	Vector3 Transform::position::get() { return Utility::Convert(trans->GetPosition()); }
	void Transform::position::set(Vector3 value) { trans->SetPosition(Utility::Convert(value)); OnPropertyChanged("localPosition"); }

	Vector3 Transform::localPosition::get() { return Utility::Convert(trans->GetLocalPosition()); }
	void Transform::localPosition::set(Vector3 value) { trans->SetLocalPosition(Utility::Convert(value));}

	Quaternion Transform::rotation::get() { return Utility::Convert(trans->GetRotation()); }
	void Transform::rotation::set(Quaternion value) { trans->SetRotation(Utility::Convert(value));}

	Quaternion Transform::localRotation::get() { return Utility::Convert(trans->GetLocalRotation()); }
	void Transform::localRotation::set(Quaternion value) { trans->SetLocalRotation(Utility::Convert(value)); }

	Vector3 Transform::eulerAngles::get() { return Utility::Convert(trans->GetEulerAngles()); }
	void Transform::eulerAngles::set(Vector3 value) { trans->SetRotation(value.y, value.x,  value.z); OnPropertyChanged("localEulerAngles");}

	Vector3 Transform::localEulerAngles::get() { return Utility::Convert(trans->GetLocalEulerAngles()); }
	void Transform::localEulerAngles::set(Vector3 value) { trans->SetLocalRotation(Utility::Convert(value)); }

	Vector3 Transform::scale::get() { return Utility::Convert(trans->GetScale()); }
	void Transform::scale::set(Vector3 value) { trans->SetScale(Utility::Convert(value));  OnPropertyChanged("localScale");}

	Vector3 Transform::localScale::get() { return Utility::Convert(trans->GetLocalScale()); }
	void Transform::localScale::set(Vector3 value) { trans->SetLocalScale(Utility::Convert(value));  OnPropertyChanged("localScale"); }

	Vector3 Transform::forward::get() { return Utility::Convert(trans->Forward()); }
	Vector3 Transform::up::get() { return Utility::Convert(trans->Up()); }
	Vector3 Transform::right::get() { return Utility::Convert(trans->Right()); }
	Vector3 Transform::local_forward::get() { return Utility::Convert(trans->GetLocalWorldMatrix().Forward()); }
	Vector3 Transform::local_up::get() { return Utility::Convert(trans->GetLocalWorldMatrix().Up()); }
	Vector3 Transform::local_right::get() { return Utility::Convert(trans->GetLocalWorldMatrix().Right()); }



	thomas::object::component::Transform* Transform::Native::get()
	{
		return reinterpret_cast<thomas::object::component::Transform*>(nativePtr);
	}

	void Transform::LookAt(Transform^ target) {
		trans->LookAt((thomas::object::component::Transform*)nativePtr);
	}

	void Transform::LookAt(Vector3 target) {
		trans->LookAt(Utility::Convert(target));
	}

	void Transform::Rotate(Vector3 angles) {
		trans->Rotate(Utility::Convert(angles));
	}

	void Transform::Rotate(float x, float y, float z) {
		Rotate(Vector3(x, y, z));
	}

	void Transform::RotateByAxis(Vector3 axis, float angle) {
		trans->RotateByAxis(Utility::Convert(axis), angle);
	}

	void Transform::Translate(Vector3 translation) {
		trans->Translate(Utility::Convert(translation));
	}
	void Transform::Translate(float x, float y, float z) {
		Translate(Vector3(x, y, z));
	}

	bool Transform::IsChildOf(Transform^ _parent)
	{
		if (this->nativePtr == _parent->nativePtr)
			return true;
		else if (parent != nullptr)
			return parent->IsChildOf(_parent);
		else
			return false;
	}

	void Transform::Orient(Vector3 forward, Vector3 up)
	{
		trans->Orient(Utility::Convert(forward), Utility::Convert(up));
	}

	void Transform::OnDestroy()
	{
		for (int i = 0; i < m_children->Count; i++)
			m_children[i]->OnParentDestroy(m_parent);
		m_children->Clear();
		SetParent(nullptr);
	}

	void Transform::Update()
	{
		if (trans->IsDirty())
		{
			trans->SetDirty(false);
			OnPropertyChanged("localPosition");
			OnPropertyChanged("localEulerAngles");
			OnPropertyChanged("localScale");
		}
	}
}

