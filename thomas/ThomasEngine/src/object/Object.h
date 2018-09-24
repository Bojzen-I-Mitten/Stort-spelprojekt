#pragma once
#pragma unmanaged
#include <thomas\object\Object.h>
#pragma managed
#include "../Utility.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Linq;
namespace ThomasEngine {

	[SerializableAttribute]
	public ref class Object: public INotifyPropertyChanged
	{
		static List<Object^> s_objects;
	private:
		
	protected:
		
		String^ m_name;
	internal:
		[System::Runtime::Serialization::DataMemberAttribute]
		Guid m_guid;

		[NonSerializedAttribute]
		thomas::object::Object* nativePtr;

		static Object^ Find(Guid guid)
		{
			for each(Object^ o in s_objects)
			{
				if (o->m_guid == guid)
					return o;
			}
			return nullptr;
		}


	public:

		[NonSerializedAttribute]
		bool m_loading = false;

		[field:NonSerializedAttribute]
		virtual event PropertyChangedEventHandler^ PropertyChanged;
		virtual void OnDestroy() {nativePtr->OnDestroy(); }

		static void Destroy(Object^ object) { object->Destroy(); }
		
		void OnPropertyChanged(String^ info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}

		[BrowsableAttribute(false)]
		virtual property String^ Name
		{
			String^ get() { return m_name; }

			void set(String^ value)
			{
				m_name = value;
				OnPropertyChanged("Name");
			}
		}

		Object(thomas::object::Object* ptr)
		{
			nativePtr = ptr;
			s_objects.Add(this);
			thomas::object::Object::Add(ptr);
			m_guid = Guid::NewGuid();
			nativePtr->m_guid = Utility::Convert(m_guid);
		}

		virtual void Destroy()
		{
			OnDestroy();
			thomas::object::Object::Destroy(nativePtr);
			s_objects.Remove(this);
		}

		static Object^ GetObject(thomas::object::Object* ptr)
		{
			for(int i=0; i < s_objects.Count; i++)
			{
				if (s_objects[i]->nativePtr == ptr)
					return s_objects[i];
			}
			return nullptr;
		}


		static List<Object^>^ GetObjects()
		{
			return %s_objects;
		}
		generic<typename T>
		where T: Object
		static List<T>^ GetObjectsOfType() {
			return gcnew List<T>(Enumerable::OfType<T>(GetObjectsOfType(T::typeid)));
			
		}
		static List<Object^>^ GetObjectsOfType(Type^ type);

		static bool operator ==(Object^ a, Object^ b)
		{
			if (Object::ReferenceEquals(nullptr, a))
				return Object::ReferenceEquals(nullptr, b);

			if (Object::ReferenceEquals(nullptr, b))
				return false;

			return a->nativePtr == b->nativePtr;
		}

		static bool operator !=(Object^ a, Object^ b)
		{

			if (Object::ReferenceEquals(nullptr, a))
				return !Object::ReferenceEquals(nullptr, b);

			if (Object::ReferenceEquals(nullptr, b))
				return true;

			return a->nativePtr != b->nativePtr;
			
		}

		static operator bool(Object^ object)
		{
			return object != nullptr;
		}

		[System::Runtime::Serialization::OnDeserializedAttribute]
		void OnDeserializedObject(System::Runtime::Serialization::StreamingContext c)
		{
			nativePtr->m_guid = Utility::Convert(m_guid);
			m_loading = false;
		}
	};
}