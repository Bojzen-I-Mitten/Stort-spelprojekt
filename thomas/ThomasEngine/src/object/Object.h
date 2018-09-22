#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
namespace thomas{ namespace object { class Object; } }
namespace ThomasEngine {

	[System::SerializableAttribute]
	public ref class Object: public System::ComponentModel::INotifyPropertyChanged
	{
		static List<Object^> s_objects;
	private:
		
	protected:
		
		System::String^ m_name;
	internal:
		[System::Runtime::Serialization::DataMemberAttribute]
		System::Guid m_guid;

		[System::NonSerializedAttribute]
		thomas::object::Object* nativePtr;

		static Object^ Find(System::Guid guid);


	public:

		Object(thomas::object::Object* ptr);

		[field:NonSerializedAttribute]
		virtual event PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(System::String^ info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}

		virtual void OnDestroy();

		static void Destroy(Object^ object) { object->Destroy(); }
		
		virtual void Destroy();

		static Object^ GetObject(thomas::object::Object* ptr);


		static List<Object^>^ GetObjects();
		static List<Object^>^ GetObjectsOfType(System::Type^ type);

		generic<typename T>
		where T: Object
		static List<T>^ GetObjectsOfType() {
			return gcnew List<T>(System::Linq::Enumerable::OfType<T>(GetObjectsOfType(T::typeid)));
		}


		[BrowsableAttribute(false)]
		virtual property System::String^ Name
		{
			System::String^ get() { return m_name; }

			void set(System::String^ value)
			{
				m_name = value;
				OnPropertyChanged("Name");
			}
		}
		static bool operator ==(Object^ a, Object^ b);

		static bool operator !=(Object^ a, Object^ b);

		static operator bool(Object^ object);

		[System::Runtime::Serialization::OnDeserializedAttribute]
		void OnDeserializedObject(System::Runtime::Serialization::StreamingContext c);
	};
}