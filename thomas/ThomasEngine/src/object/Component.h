#pragma once
#pragma unmanaged
#include <thomas\object\component\Component.h>
#pragma managed

#include "../attributes/CustomAttributes.h"
#include <string>
#include <msclr\marshal_cppstd.h>
#include "Object.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;

namespace ThomasEngine 
{
	ref class GameObject;
	ref class Transform;
	[HideInInspector]
	public ref class Component : public Object
	{
		Component();

	internal:
		static List<System::Type^>^ externalTypes = gcnew List<System::Type^>();
		static void LoadExternalComponents();

		Component(thomas::object::component::Component* ptr);
		
		void setGameObject(GameObject^ gObj);
		virtual void Awake() { ((thomas::object::component::Component*)nativePtr)->Awake(); }
		virtual void Start() {};
		virtual void OnEnable() { ((thomas::object::component::Component*)nativePtr)->OnEnable(); }
		virtual void OnDisable() { ((thomas::object::component::Component*)nativePtr)->OnDisable(); };
		virtual void Update() { ((thomas::object::component::Component*)nativePtr)->Update(); }
		virtual void FixedUpdate() {((thomas::object::component::Component*)nativePtr)->FixedUpdate();}
		virtual void OnDrawGizmosSelected() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmosSelected(); }
		virtual void OnDrawGizmos() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmos(); }

		GameObject^ m_gameObject;

		
		property bool initialized
		{
			bool get() { return ((thomas::object::component::Component*)nativePtr)->initialized; }
			void set(bool value) { ((thomas::object::component::Component*)nativePtr)->initialized = value; }
		}

		void Initialize() {
			if (!awakened)
			{
				Awake();
				awakened = true;
				return;
			}
			else if (!enabled) {
				OnEnable();
				enabled = true;
				return;
			}else{
				Start();
				initialized = true;
				return;
			}
			
		}
		bool awakened = false;

	public:
		static System::Reflection::Assembly^ editorAssembly;

		[Xml::Serialization::XmlIgnoreAttribute]
		bool enabled = false;

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