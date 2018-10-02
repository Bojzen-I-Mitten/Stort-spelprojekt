#pragma once
#include "../Component.h"

namespace thomas { namespace object { namespace component { class Transform; } } }
namespace ThomasEngine
{
	[DisallowMultipleComponent]
	[ExecuteInEditor]
	[HideInInspector]
	public ref class Transform : public Component
	{
	private:
		property thomas::object::component::Transform* trans { thomas::object::component::Transform* get(); }
	public:
		Transform();

		delegate void ParentChanged(Transform^ child, Transform^ oldParent, Transform^ newParent);
		static event ParentChanged^ OnParentChanged;

		[Xml::Serialization::XmlIgnoreAttribute]
		[BrowsableAttribute(false)]
		virtual property bool canDisable
		{
			virtual bool get() override { return false; }
		}
		
		[BrowsableAttribute(false)]
		property Transform^ parent 
		{
			Transform^ get();

			void set(Transform^ value);
		}

		[BrowsableAttribute(false)]
		property List<Transform^>^ children 
		{
			List<Transform^>^ get();
		}

		 
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 position
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Matrix world
		{
			Matrix get();
			void set(Matrix value);
		}


		[DisplayNameAttribute("position")]
		property Vector3 localPosition
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Quaternion rotation
		{
			Quaternion get();
			void set(Quaternion value);
		}
	
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 eulerAngles
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[DisplayNameAttribute("rotation")]
		property Vector3 localEulerAngles
		{
			Vector3 get();
			void set(Vector3 value);
		}
		
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 scale
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[DisplayNameAttribute("scale")]
		property Vector3 localScale
		{
			Vector3 get();
			void set(Vector3 value);
		}
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 forward
		{
			Vector3 get();
		}
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 up
		{
			Vector3 get();
		}
		[System::Runtime::Serialization::IgnoreDataMemberAttribute]
		[BrowsableAttribute(false)]
		property Vector3 right
		{
			Vector3 get();
		}

		void LookAt(Transform^ target);

		void LookAt(Vector3 target);

		void Rotate(Vector3 angles);

		void Rotate(float x, float y, float z);

		void RotateByAxis(Vector3 axis, float angle);

		void Translate(Vector3 translation);
		void Translate(float x, float y, float z);

		bool IsChildOf(Transform^ _parent);

		void SetParent(Transform^ value);
		void SetParent(Transform^ value, bool worldPositionStays);

		void OnDestroy() override;
		void Update() override;
	};
}