#pragma once

#pragma managed
using namespace System::Reflection;
using namespace System::Collections;

namespace ThomasEngine
{
	public interface class IPropertyChanged
	{
	public:
		virtual void PropertyDataChanged(System::Type^ info, System::Object^ data) = 0;
	};
}