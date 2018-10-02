#pragma unmanaged
#include <thomas\object\component\Transform.h>
#pragma managed
#include "Transform.h"
#include "../GameObject.h"
#include "../../Utility.h"
#include "../../ThomasManaged.h"

namespace ThomasEngine
{
	Transform::Transform() : Component(new thomas::object::component::Transform()){}

	thomas::object::component::Transform* Transform::trans::get() { return (thomas::object::component::Transform*)nativePtr; }



	Transform^ Transform::parent::get()
	{
		if (trans->GetParent())
			return (Transform^)GetObject(trans->GetParent());
		else
			return nullptr;
	}
	void Transform::parent::set(ThomasEngine::Transform^ value)
	{

		SetParent(value);

	}

	List<Transform^>^ Transform::children::get()
	{
		std::vector<thomas::object::component::Transform*> nativeChildren = trans->GetChildren();
		List<Transform^>^ managedChildren = gcnew List<Transform^>(int(nativeChildren.size()));

		for (thomas::object::component::Transform* nativeChild : nativeChildren)
			managedChildren->Add((Transform^)GetObject(nativeChild));

		return managedChildren;
	}

	Matrix Transform::world::get(){ return Utility::Convert(trans->GetWorldMatrix());}
	void Transform::world::set(Matrix value)
	{
		trans->SetWorldMatrix(Utility::Convert(value));
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

	Vector3 Transform::eulerAngles::get() { return Utility::Convert(trans->GetEulerAngles()); }
	void Transform::eulerAngles::set(Vector3 value) { trans->SetRotation(value.y, value.x,  value.z); OnPropertyChanged("localEulerAngles");}

	Vector3 Transform::localEulerAngles::get() { return Utility::Convert(trans->GetLocalEulerAngles()); }
	void Transform::localEulerAngles::set(Vector3 value) { trans->SetLocalRotation(value.y, value.x,  value.z); }

	Vector3 Transform::scale::get() { return Utility::Convert(trans->GetScale()); }
	void Transform::scale::set(Vector3 value) { trans->SetScale(Utility::Convert(value));  OnPropertyChanged("localScale");}

	Vector3 Transform::localScale::get() { return Utility::Convert(trans->GetLocalScale()); }
	void Transform::localScale::set(Vector3 value) { trans->SetLocalScale(Utility::Convert(value));  OnPropertyChanged("localScale"); }

	Vector3 Transform::forward::get() { return Utility::Convert(trans->Forward()); }
	Vector3 Transform::up::get() { return Utility::Convert(trans->Up()); }
	Vector3 Transform::right::get() { return Utility::Convert(trans->Right()); }


	void Transform::SetParent(Transform ^ value)
	{
		SetParent(value, false);
	}
	void Transform::SetParent(Transform ^ value, bool worldPositionStays)
	{
		Transform^ oldParent = parent;
		if (value)
			((thomas::object::component::Transform*)nativePtr)->SetParent((thomas::object::component::Transform*)value->nativePtr, worldPositionStays);
		else
			((thomas::object::component::Transform*)nativePtr)->SetParent(nullptr, worldPositionStays);

		OnParentChanged(this, oldParent, value);
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

	void Transform::OnDestroy()
	{
		if (parent)
		{
			parent = nullptr;
		}
		for (int i = 0; i < children->Count; i++)
		{
			children[i]->gameObject->Destroy();
			i -= 1;
		}
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

