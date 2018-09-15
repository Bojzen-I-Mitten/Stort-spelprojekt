#pragma once

#include "../attributes/CustomAttributes.h"
#include "Object.h"

namespace thomas { namespace object { namespace component { class Component; } } }

namespace ThomasEngine 
{
	ref class GameObject;
	ref class Transform;
	[HideInInspector]
	[SerializableAttribute]
	public ref class Component : public Object
	{
		Component();
	private:
		[NonSerializedAttribute]
		bool m_enabled = false;
	internal:
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
			bool get() { return m_enabled; }
			void set(bool value) {
				if (m_enabled != value) {
					m_enabled = value;
					if (value == true)
						OnEnable();
					else
						OnDisable();
				}
			}
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
	};
}