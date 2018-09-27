#include "Math.h"


namespace DirectX
{
	namespace SimpleMath
	{
		Vector3 ToEuler(const Quaternion & q)
		{
			float yaw, pitch, roll;

			yaw = (float)std::atan2f(2.f * q.y * q.w - 2.f * q.x * q.z,1.f - 2.f * square(q.y) - 2.f * square(q.z));

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
	}
}
