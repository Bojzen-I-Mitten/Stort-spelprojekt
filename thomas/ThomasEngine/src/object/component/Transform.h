#pragma once
#pragma unmanaged
#include <thomas\object\component\Transform.h>
#pragma managed
#include "../../Utility.h"
#include "../Component.h"
#include <string>
#include <msclr\marshal_cppstd.h>

using namespace System::ComponentModel;
namespace ThomasEngine
{
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	[HideInInspector]
	public ref class Transform : public Component
	{
	public:
		Transform() : Component(new thomas::object::component::Transform()) 
		{
		}

		[BrowsableAttribute(false)]
		property Transform^ parent 
		{
			Transform^ get() 
			{
				if (((thomas::object::component::Transform*)nativePtr)->GetParent())
					return (Transform^)GetObject(((thomas::object::component::Transform*)nativePtr)->GetParent());
				else
					return nullptr;
				
			}

			void set(Transform^ value);
		}

		[BrowsableAttribute(false)]
		property List<Transform^>^ children 
		{
			List<Transform^>^ get() 
			{
				std::vector<thomas::object::component::Transform*> nativeChildren = ((thomas::object::component::Transform*)nativePtr)->GetChildren();
				List<Transform^>^ managedChildren = gcnew List<Transform^>(int(nativeChildren.size()));

				for (thomas::object::component::Transform* nativeChild : nativeChildren)
					managedChildren->Add((Transform^)GetObject(nativeChild));

				return managedChildren;
			}
		}

		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 position
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetPosition()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetPosition(thomas::math::Vector3(value.x, value.y, value.z)); OnPropertyChanged("localPosition");}
		}

		[DisplayNameAttribute("position")]
		property Vector3 localPosition
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetLocalPosition()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetLocalPosition(value.x, value.y, value.z); }
		}

		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Quaternion rotation
		{
			Quaternion get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetRotation()); }
			void set(Quaternion value) { ((thomas::object::component::Transform*)nativePtr)->SetRotation(thomas::math::Quaternion(value.x, value.y, value.z, value.w)); OnPropertyChanged("localEulerAngles");}
		}
	
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 eulerAngles
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetEulerAngles()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetRotation(value.y, value.x, value.z); OnPropertyChanged("localEulerAngles");}
		}

		
		[DisplayNameAttribute("rotation")]
		property Vector3 localEulerAngles
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetLocalEulerAngles()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetLocalRotation(value.y, value.x, value.z); }
		}
		
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 scale
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetScale()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetScale(thomas::math::Vector3(value.x, value.y, value.z)); OnPropertyChanged("localScale");}
		}

		[DisplayNameAttribute("scale")]
		property Vector3 localScale
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->GetLocalScale()); }
			void set(Vector3 value) { ((thomas::object::component::Transform*)nativePtr)->SetLocalScale(thomas::math::Vector3(value.x, value.y, value.z)); }
		}
		[BrowsableAttribute(false)]
		property Vector3 forward
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->Forward()); }
		}
		[BrowsableAttribute(false)]
		property Vector3 up
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->Up()); }
		}
		[BrowsableAttribute(false)]
		property Vector3 right
		{
			Vector3 get() { return Utility::Convert(((thomas::object::component::Transform*)nativePtr)->Right()); }
		}

		void LookAt(Transform^ target) {
			((thomas::object::component::Transform*)nativePtr)->LookAt((thomas::object::component::Transform*)nativePtr);
		}

		void LookAt(Vector3 target) {
			((thomas::object::component::Transform*)nativePtr)->LookAt(Utility::Convert(target));
		}

		void Rotate(Vector3 angles) {
			((thomas::object::component::Transform*)nativePtr)->Rotate(Utility::Convert(angles));
		}

		void Rotate(float x, float y, float z) {
			Rotate(Vector3(x, y, z));
		}

		void RotateByAxis(Vector3 axis, float angle) {
			((thomas::object::component::Transform*)nativePtr)->RotateByAxis(Utility::Convert(axis), angle);
		}

		void Translate(Vector3 translation) {
			((thomas::object::component::Transform*)nativePtr)->Translate(Utility::Convert(translation));
		}
		void Translate(float x, float y, float z) {
			Translate(Vector3(x, y, z));
		}

		bool IsChildOf(Transform^ _parent)
		{
			if (this->nativePtr == _parent->nativePtr)
				return true;
			else if (parent != nullptr)
				return parent->IsChildOf(_parent);
			else
				return false;
		}

		void OnDestroy() override;
		void Update() override
		{
			if (((thomas::object::component::Transform*)nativePtr)->IsDirty())
			{
				((thomas::object::component::Transform*)nativePtr)->SetDirty(false);
				OnPropertyChanged("localPosition");
				OnPropertyChanged("localEulerAngles");
				OnPropertyChanged("localScale");
			}
		}	
	};
}