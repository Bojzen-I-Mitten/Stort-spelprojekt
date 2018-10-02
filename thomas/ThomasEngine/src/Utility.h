#pragma once

#pragma unmanaged
#include <thomas/utils/Math.h>
#pragma managed


namespace ThomasEngine
{
	class Utility
	{
	public:

		static uint32_t hash(System::String^ string);

		static std::string ConvertString(System::String^ string);

		static System::String^ ConvertString(std::string string);

		static thomas::math::Color Convert(Color% value)
		{
			return thomas::math::Color(Utility::Convert(value.ToVector4()));
		}
		static Color Convert(thomas::math::Color& value)
		{
			return Color(value.x, value.y, value.z, value.w);
		}
		static thomas::math::Vector2 Convert(Vector2% value)
		{
			return thomas::math::Vector2(value.x, value.y);
		}
		static Vector2 Convert(thomas::math::Vector2& value)
		{
			return Vector2(value.x, value.y);
		}
		static thomas::math::Vector3 Convert(Vector3% value)
		{
			return thomas::math::Vector3(value.x, value.y, value.z);
		}
		static Vector3 Convert(thomas::math::Vector3& value)
		{
			return Vector3(value.x, value.y, value.z);
		}
		static thomas::math::Vector4 Convert(Vector4% value)
		{
			return thomas::math::Vector4(value.x, value.y, value.z, value.w);
		}
		static Vector4 Convert(thomas::math::Vector4& value)
		{
			return Vector4(value.x, value.y, value.z, value.w);
		}
		static thomas::math::Quaternion Convert(Quaternion% value)
		{
			return thomas::math::Quaternion(value.x, value.y, value.z, value.w);
		}
		static Quaternion Convert(thomas::math::Quaternion% value)
		{
			return Quaternion(value.x, value.y, value.z, value.w);
		}

		static _GUID Convert(System::Guid% value);

		static System::Guid Convert(_GUID& value);

		static thomas::math::Matrix Convert(Matrix% value)
		{
			return thomas::math::Matrix(
				value.M11, value.M12, value.M13, value.M14,
				value.M21, value.M22, value.M23, value.M24,
				value.M31, value.M32, value.M33, value.M34,
				value.M41, value.M42, value.M43, value.M44
			);
		}
		static thomas::math::Matrix Convert(const Matrix% value)
		{
			return thomas::math::Matrix(
				value.M11, value.M12, value.M13, value.M14,
				value.M21, value.M22, value.M23, value.M24,
				value.M31, value.M32, value.M33, value.M34,
				value.M41, value.M42, value.M43, value.M44
			);
		}

		static Matrix Convert(thomas::math::Matrix& value)
		{
			return Matrix(
				value.m[0][0], value.m[0][1], value.m[0][2], value.m[0][3],
				value.m[1][0], value.m[1][1], value.m[1][2], value.m[1][3],
				value.m[2][0], value.m[2][1], value.m[2][2], value.m[2][3],
				value.m[3][0], value.m[3][1], value.m[3][2], value.m[3][3]
				);
		}
		static Matrix Convert(const thomas::math::Matrix& value)
		{
			return Matrix(
				value.m[0][0], value.m[0][1], value.m[0][2], value.m[0][3],
				value.m[1][0], value.m[1][1], value.m[1][2], value.m[1][3],
				value.m[2][0], value.m[2][1], value.m[2][2], value.m[2][3],
				value.m[3][0], value.m[3][1], value.m[3][2], value.m[3][3]
			);
		}
	};

}