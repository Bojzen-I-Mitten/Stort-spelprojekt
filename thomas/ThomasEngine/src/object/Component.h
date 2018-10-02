#pragma once

#include "../attributes/CustomAttributes.h"
#include "Object.h"

namespace thomas { namespace object { namespace component { class Component; } } }

namespace ThomasEngine 
{
	ref class GameObject;
	ref class Collider;
	ref class Transform;
	[HideInInspector]
	[SerializableAttribute]
	public ref class Component : public Object
	{
		Component();
	private:
		bool m_enabled = true;
		[NonSerializedAttribute]
		bool m_started = false;
	internal:
		[NonSerializedAttribute]
		bool m_firstEnable = false;

		[NonSerializedAttribute]
		List<System::Collections::IEnumerator^>^ coroutines = gcnew List<System::Collections::IEnumerator^>();
		void UpdateCoroutines();

		static List<System::Type^>^ externalTypes = gcnew List<System::Type^>();
		static void LoadExternalComponents();

		Component(thomas::object::component::Component* ptr);
		
		void setGameObject(GameObject^ gObj);
		virtual void Awake();
		virtual void Start();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void OnDrawGizmosSelected();
		virtual void OnDrawGizmos();

		virtual void OnCollisionEnter(Collider^ collider) {};
		virtual void OnCollisionStay(Collider^ collider) {};
		virtual void OnCollisionExit(Collider^ collider) {};

		virtual void OnTriggerEnter(Collider^ collider) {};
		virtual void OnTriggerStay(Collider^ collider) {};
		virtual void OnTriggerExit(Collider^ collider) {};

		GameObject^ m_gameObject;

		
		property bool initialized
		{
			bool get();
			void set(bool value);
		}

		void Initialize();
		[NonSerializedAttribute]
		bool awakened = false;

	public:
		static System::Reflection::Assembly^ editorAssembly;
		
		[Xml::Serialization::XmlIgnoreAttribute]
		[BrowsableAttribute(false)]
		property bool enabled {
			bool get();
			void set(bool value);
		}

		[Xml::Serialization::XmlIgnoreAttribute]
		[BrowsableAttribute(false)]
		virtual property bool canDisable
		{
			virtual bool get() { return true; }
		}


		[BrowsableAttribute(false)]
		property GameObject^ gameObject
		{
			GameObject^ get() { return m_gameObject; }
			void set(GameObject^ value) { setGameObject(value); }
		}

		[BrowsableAttribute(false)]
		property Transform^ transform
		{
			Transform^ get();
			
		}

		[BrowsableAttribute(false)]
		property String^ Name
		{
			String^ get() override;
		};

		virtual void Destroy() override;

		static List<Type^>^ GetAllComponentTypes();
		static List<Type^>^ GetAllAddableComponentTypes();

		void StartCoroutine(System::Collections::IEnumerator^ routine);
		void StopCoroutine(System::Collections::IEnumerator^ routine);
		void StopAllCoroutines();

	};
}