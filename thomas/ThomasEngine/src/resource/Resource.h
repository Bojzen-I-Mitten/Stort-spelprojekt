#pragma once

#pragma managed
#include "../Utility.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::Serialization;
namespace thomas {
	namespace resource {
		class Resource;
	} 
}
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class Resource : public System::ComponentModel::INotifyPropertyChanged
	{
	internal:
		thomas::resource::Resource* m_nativePtr;

		[DataMemberAttribute(Order = 0)]
		property System::String^ asset_path {
			System::String^ get() { return GetAssetRelativePath(); }
			void set(System::String^ value);
		}
		System::String^ m_path;

		Resource(System::String^ path, thomas::resource::Resource* ptr)
		{
			m_path = path;
			m_nativePtr = ptr;
		}
		virtual ~Resource()
		{
		}

		virtual void OnPlay() {};
		virtual void OnStop() {};

		void Rename(System::String^ newPath);

	public:
		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(System::String^ info)
		{
			PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(info));
		}

		virtual void Reload();

		System::String ^ GetPath()
		{
			return m_path;
		}

		virtual property System::String^ Name;

		System::String^ GetAssetRelativePath();

		virtual property System::String^ name
		{
			System::String^ get() { return System::IO::Path::GetFileNameWithoutExtension(m_path); }
		};

		virtual System::String^ ToString() override
		{
			return Name;
		}

		static bool operator ==(Resource^ a, Resource^ b)
		{
			if (Object::ReferenceEquals(nullptr, a))
				return Object::ReferenceEquals(nullptr, b);

			if (Object::ReferenceEquals(nullptr, b))
				return false;

			return a->m_nativePtr == b->m_nativePtr;
		}

		static bool operator !=(Resource^ a, Resource^ b)
		{

			if (Object::ReferenceEquals(nullptr, a))
				return !Object::ReferenceEquals(nullptr, b);

			if (Object::ReferenceEquals(nullptr, b))
				return true;

			return a->m_nativePtr != b->m_nativePtr;

			
		}
	};
}