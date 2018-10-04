#include "Math.h"


namespace DirectX
{
	namespace SimpleMath
	{
		Vector3 ToEuler(const Quaternion & q)
		{
			float yaw, pitch, roll;

			yaw = (float)std::atan2f(2.f * q.y * q.w - 2.f * q.x * q.z, 1.f - 2.f * square(q.y) - 2.f * square(q.z));

			roll = std::asinf(2.f * q.x * q.y + 2.f * q.z * q.w);

			pitch = std::atan2f(2.f * q.x * q.w - 2.f * q.y * q.z, 1.f - 2.f * square(q.x) - 2.f * square(q.z));

			if (q.x * q.y + q.z * q.w == 0.5f)
			{
				yaw = (float)(2.f * std::atan2f(q.x, q.w));
				pitch = 0.f;
			}

			else if (q.x * q.y + q.z * q.w == -0.5f)
			{
				yaw = (float)(-2.f * std::atan2f(q.x, q.w));
				pitch = 0.f;
			}

			return Vector3(RadiansToDegrees(pitch), RadiansToDegrees(yaw), RadiansToDegrees(roll));
		}
		Quaternion FromEuler(const Vector3 & rotation)
		{
			return Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
		}


		Quaternion getRotationTo(Vector3 from, Vector3 dest)
		{
			// Based on Stan Melax's article in Game Programming Gems
			Quaternion q;
			from.Normalize();
			dest.Normalize();

			float d = from.Dot(dest);
			if (d >= 1.0f) // Vectors are parallel	(identical)
				return Quaternion::Identity;
			if (d < (1e-6f - 1.0f)) // Vectors are parallel (opposite)
			{
				// Generate an axis
				Vector3 axis = Vector3::UnitX.Cross(from);
				if (axis.LengthSquared() < 0.0001f) // Pick another if colinear
					axis = Vector3::UnitY.Cross(from);
				axis.Normalize();
				q = Quaternion::CreateFromAxisAngle(axis, PI);
			}
			else
			{
				float s = std::sqrtf((1 + d) * 2);
				float invs = 1 / s;

				Vector3 c = from.Cross(dest);

				q.x = c.x * invs;
				q.y = c.y * invs;
				q.z = c.z * invs;
				q.w = s * 0.5f;
				q.Normalize();
			}
			return q;
		}
	}
}
