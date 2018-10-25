#pragma once
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
		virtual ~Resource();

		virtual void OnPlay() {};
		virtual void OnStop() {};

		/* Update file path of the resource
		*/
		void Rename(String^ newPath);

	public:
		virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(System::String^ info)
		{
			PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(info));
		}

		virtual void Reload();
		virtual void OnRename() {};
		System::String ^ GetPath()
		{
			return m_path;
		}


		System::String^ GetAssetRelativePath();

		virtual property String^ Name
		{
			System::String^ get() { return System::IO::Path::GetFileNameWithoutExtension(m_path); }
		};

		virtual System::String^ ToString() override
		{
			return Name;
		}

		static bool operator ==(Resource^ a, Resource^ b);

		static bool operator !=(Resource^ a, Resource^ b);
	};
}