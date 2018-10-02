#include "Utility.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <msclr\marshal_cppstd.h>


namespace ThomasEngine
{

	unsigned long	hash_djb2(const char *str)
	{
		unsigned long hash = 5381;
		int c;

		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
	}


	uint32_t Utility::hash(System::String ^ string)
	{
		std::string str = Utility::ConvertString(string);
		return hash_djb2(str.c_str());
	}
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