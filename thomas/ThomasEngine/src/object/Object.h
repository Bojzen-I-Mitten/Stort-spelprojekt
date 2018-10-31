#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
namespace thomas{ namespace object { class Object; } }
namespace ThomasEngine {

	[System::SerializableAttribute]
	public ref class Object: public System::ComponentModel::INotifyPropertyChanged
	{
		
	private:
		/* Silent destruction. */
		void Delete();

#ifdef _DEBUG
		bool m_Destroyed = false;
#endif
		
	protected:
		static List<Object^> s_objects;
		System::String^ m_name;
	internal:
		[System::Runtime::Serialization::DataMemberAttribute]
		System::Guid m_guid;

		[System::NonSerializedAttribute]
		thomas::object::Object* nativePtr;

		static Object^ Find(System::Guid guid);

		virtual void Destroy();

	public:

		Object(thomas::object::Object* ptr);
		virtual ~Object();


		[field:NonSerializedAttribute]
		virtual event PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(System::String^ info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}

		static void Destroy(Object^ object) { object->Destroy(); }
	

		virtual void OnDestroy() {};
		

	public:
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