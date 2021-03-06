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
	public:
		delegate void ParentChanged(Transform^ child, Transform^ oldParent, Transform^ newParent);
		static event ParentChanged^ OnParentChanged;
	private:
		property thomas::object::component::Transform* trans { thomas::object::component::Transform* get(); }
		Transform^ m_parent;
		List<Transform^>^ m_children;

		void AddChild(Transform^ child);
		bool RemoveChild(Transform^ child);
		Transform^ SwapParent(Transform ^ value, bool worldPositionStays);
		void OnParentDestroy(Transform ^ value);
	public:
		Transform();

		void Update() override;
		bool IsChildOf(Transform^ _parent);
		void OnDestroy() override;
		void OnParentDestroy(GameObject ^ parented_object) override;

		void SetParent(Transform^ value);
		void SetParent(Transform^ value, bool worldPositionStays);

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property thomas::object::component::Transform* Native { thomas::object::component::Transform* get(); }


		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		virtual property bool canDisable
		{
			virtual bool get() override { return false; }
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property IEnumerable<Transform^>^ children
		{
			IEnumerable<Transform^>^ get();
		}

		bool ShouldSerializechildren() { return false; }
		
	public:	// Serialized properties
		[Newtonsoft::Json::JsonPropertyAttribute(Order = 1)]
		[DisplayNameAttribute("scale")]
		property Vector3 localScale
		{
			Vector3 get();
			void set(Vector3 value);
		}
		[Newtonsoft::Json::JsonPropertyAttribute(Order = 2)]
		[DisplayNameAttribute("rotation")]
		property Vector3 localEulerAngles
		{
			Vector3 get();
			void set(Vector3 value);
		}
		[Newtonsoft::Json::JsonPropertyAttribute(Order = 3)]
		[DisplayNameAttribute("position")]
		property Vector3 localPosition
		{
			Vector3 get();
			void set(Vector3 value);
		}
		[Newtonsoft::Json::JsonPropertyAttribute(Order = 4, IsReference = true)]
		[BrowsableAttribute(false)]
		property Transform^ parent 
		{
			Transform^ get();

			void set(Transform^ value);
		}

	public:	// Get/Set rotation 


		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 position
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Matrix world
		{
			Matrix get();
			void set(Matrix value);
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Matrix local_world
		{
			Matrix get();
			void set(Matrix value);
		}


		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Quaternion rotation
		{
			Quaternion get();
			void set(Quaternion value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Quaternion localRotation
		{
			Quaternion get();
			void set(Quaternion value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 eulerAngles
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 scale
		{
			Vector3 get();
			void set(Vector3 value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 forward
		{
			Vector3 get();
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 up
		{
			Vector3 get();
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 right
		{
			Vector3 get();
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 local_forward
		{
			Vector3 get();
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 local_up
		{
			Vector3 get();
		}
		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Vector3 local_right
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
		
		void Orient(Vector3 forward, Vector3 up);

	};
}