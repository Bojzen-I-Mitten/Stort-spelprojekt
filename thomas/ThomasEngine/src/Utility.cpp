#include "Utility.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <msclr\marshal_cppstd.h>
#include <thomas/utils/Utility.h>

namespace ThomasEngine
{

	uint32_t Utility::hash(System::String ^ string)
	{
		std::string str = Utility::ConvertString(string);
		return thomas::utility::hash(str);
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
	Vector3 Utility::Average(array<Vector3>^ points)
	{
		Vector3 sum = Vector3::Zero;
		for (int i = 0; i < points->Length; i++)
			sum += points[i];
		return sum / points->Length;
	}
	bool Utility::PlaneFromPoints(array<Vector3>^ points, [Out] Vector3% center, [Out] Vector3% normal)
	{
		// Nifty solution from: https://www.ilikebigbits.com/2015_03_04_plane_from_points.html
		if (points->Length < 3) 
			return false;	// Span line or a single point
		center = Average(points);
		float xx = 0, xy = 0, xz = 0;
		float yy = 0, yz = 0, zz = 0;
		for (int i = 0; i < points->Length; i++)
		{
			Vector3 p = Vector3::Subtract(points[i], center);
			xx += p.x * p.x;
			xy += p.x * p.y;
			xz += p.x * p.z;
			yy += p.y * p.y;
			yz += p.y * p.z;
			zz += p.z * p.z;
		}
		/* Calculate 2x2 determinant using Cramer's rule.
		 * Fit solution to all axis and use the largest determinant to select the result.
		*/
		float det_x = yy * zz - yz * yz;
		float det_y = xx * zz - xz * xz;
		float det_z = xx * yy - xy * xy;

		float det_max = std::fmax(std::fmax(det_x, det_y), det_z);
		if (det_max <= 0.0) {
			return false; // The points don't span a plane
		}

		// Pick determinant with best conditioning:
		if (det_max == det_x)
		{
			normal.x = det_x;
			normal.y = xz * yz - xy * zz;
			normal.z = xy * yz - xz * yy;
		}
		else if (det_max == det_y)
		{
			normal.x = xz * yz - xy * zz;
			normal.y = det_y;
			normal.z = xy * xz - yz * xx;
		}
		else {
			normal.x = xy * yz - xz * yy;
			normal.y = xy * xz - yz * xx;
			normal.z = det_z;
		};
		normal.Normalize();
	}
}