#pragma once

#include "../attributes/CustomAttributes.h"
#include "Object.h"
#include "ComponentState.h"

namespace thomas { namespace object { namespace component { class Component; } } }

namespace ThomasEngine 
{
	/* Component initiation call bits
	*/
	constexpr uint32_t INIT_PLAYING_BIT = 1;			// If set: System is playing
	constexpr uint32_t INIT_EXPLICIT_CALL_BIT = 2;		// If set: Activation is called explicitly (not initial initiation).

	ref class GameObject;
	ref class Collider;
	ref class Transform;
	[HideInInspector]
	[SerializableAttribute]
	public ref class Component : public Object
	{
		Component();
	private:
		/* Tracks component state. Enabled/Disabled... */
		[NonSerializedAttribute]
		Comp::State m_state;
		/* Tracks if component is an 'Activated' in context of the editor. Does not directly indicate if it's 'enabled'. */
		bool m_active;
	internal:

		[NonSerializedAttribute]
		List<System::Collections::IEnumerator^>^ coroutines = gcnew List<System::Collections::IEnumerator^>();
		void UpdateCoroutines();

		Component(thomas::object::component::Component* ptr);
		virtual ~Component();


		void setGameObject(GameObject^ gObj);
		virtual void OnGameObjectSet() {};
		virtual void OnAwake();
		//virtual void Start();
		virtual void OnEnable();
		virtual void OnDisable();
		virtual void Start();
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

		virtual void Destroy() override;



	private:
	internal:
		/* Combined initiation logic function for component initiation
		s			<<	State to enter
		InitBits	<<	Information specific to the initiation call.
		*/
		void InitComponent(Comp::State s, uint32_t InitBits);
		/* Call to 'construct' the object */
		void Awake();
		/* Enabled the component. */
		void Enable();
		/* Disable the component. */
		void Disable();



	public:
		/* Dynamic destruction of the object callable from object handle. */
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
		/* Serialization property only (used to store deactivation)
		*/
		[BrowsableAttribute(false)]
		//[Newtonsoft::Json::JsonPropertyAttribute(Newtonsoft::Json::DefaultValueHandling = Newtonsoft::Json::DefaultValueHandling::Populate)]
		[System::ComponentModel::DefaultValueAttribute(true)]
		property bool Activated
		{
			bool get();
			void set(bool state);
		}

		[Newtonsoft::Json::JsonIgnoreAttribute]
		[BrowsableAttribute(false)]
		property Comp::State ComponentState 
		{
			Comp::State get();
		}

		/* True if object can be enabled in editor
		*/
		bool enableInEditor();

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