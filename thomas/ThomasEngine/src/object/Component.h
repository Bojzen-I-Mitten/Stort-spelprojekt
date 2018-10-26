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
		
		Component(thomas::object::component::Component* ptr);
		virtual ~Component();

		
		void setGameObject(GameObject^ gObj);
		virtual void OnGameObjectSet() {};
		virtual void Awake();
		virtual void Start();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void OnDrawGizmosSelected();
		virtual void OnDrawGizmos();
		virtual void OnParentDestroy(GameObject^ object) {};

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

	private:
		/* Set enabled state. */
		void Enable();
		/* Set disabled state. */
		void Disable();
	public:
		/* Dynamic destruction of the object callable from object handle. */
		virtual void Destroy() override;
		virtual void OnDestroy() override;

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property bool enabled {
			bool get();
			void set(bool value);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		virtual property bool canDisable
		{
			virtual bool get() { return true; }
		}


		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property GameObject^ gameObject
		{
			GameObject^ get() { return m_gameObject; }
			void set(GameObject^ value) { setGameObject(value); }
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
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

		void StartCoroutine(System::Collections::IEnumerator^ routine);
		void StopCoroutine(System::Collections::IEnumerator^ routine);
		void StopAllCoroutines();

		// Static stuff...
		internal:
			static List<System::Type^>^ externalTypes = gcnew List<System::Type^>();
			static void LoadExternalComponents();
		public:
			static System::Reflection::Assembly^ editorAssembly;
			static List<Type^>^ GetAllComponentTypes();
			static List<Type^>^ GetAllAddableComponentTypes();
	};
}