#include "Utility.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <msclr\marshal_cppstd.h>

namespace ThomasEngine
{

	std::string Utility::ConvertString(System::String^ string)
	{
		return msclr::interop::marshal_as<std::string>(string);
	}

	System::String^ Utility::ConvertString(std::string string)
	{
		return gcnew System::String(string.c_str());
	}

	_GUID Utility::Convert(System::Guid% value)
	{
		array<System::Byte > ^ guidData = value.ToByteArray();
		pin_ptr<System::Byte> data = &(guidData[0]);
		return *(_GUID*)data;
	}

	System::Guid Utility::Convert(_GUID& value)
	{
		return System::Guid(value.Data1, value.Data2, value.Data3,
			value.Data4[0], value.Data4[1],
			value.Data4[2], value.Data4[3],
			value.Data4[4], value.Data4[5],
			value.Data4[6], value.Data4[7]);
	}
}