#pragma once
#pragma unmanaged
#include "thomas\resource\Resource.h"
#include <memory>
#pragma managed
#include "../Utility.h"

using namespace System;
using namespace System::Runtime::Serialization;
using namespace System::ComponentModel;

namespace ThomasEngine
{
	ref class GameObject;
	[DataContractAttribute]
	public ref class Resource : public INotifyPropertyChanged
	{
	internal:
		thomas::resource::Resource* m_nativePtr;

		[DataMemberAttribute(Order = 0)]
		property String^ asset_path {
			String^ get() { return GetAssetRelativePath(); }
			void set(String^ value);
		}
		String^ m_path;

		Resource(String^ path, thomas::resource::Resource* ptr)
		{
			m_path = path;
			m_nativePtr = ptr;
		}
		virtual ~Resource()
		{
		}

		virtual void OnPlay() {};
		virtual void OnStop() {};

		void Rename(String^ newPath) {
			m_path = newPath;
			m_nativePtr->Rename(Utility::ConvertString(newPath));
			OnPropertyChanged("Name");
		}

	public:
		virtual event PropertyChangedEventHandler^ PropertyChanged;
		void OnPropertyChanged(String^ info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}

		virtual void Reload();

		String ^ GetPath()
		{
			return m_path;
		}

		//virtual property String^ Name;

		String^ GetAssetRelativePath();

		virtual property String^ Name
		{
			String^ get() { return System::IO::Path::GetFileNameWithoutExtension(m_path); }
		};

		virtual String^ ToString() override
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