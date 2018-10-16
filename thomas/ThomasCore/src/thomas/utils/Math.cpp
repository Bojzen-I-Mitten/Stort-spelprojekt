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


		BoundingFrustum CreateFrustrumFromMatrixRH(CXMMATRIX projection)
		{
			BoundingFrustum Out;
			// Corners of the projection frustum in homogenous space.
			static XMVECTORF32 HomogenousPoints[6] =
			{
			 { 1.0f,  0.0f, -1.0f, 1.0f },   // right (at far plane)
			 { -1.0f,  0.0f, -1.0f, 1.0f },   // left
			 { 0.0f,  1.0f, -1.0f, 1.0f },   // top
			 { 0.0f, -1.0f, -1.0f, 1.0f },   // bottom

			 { 0.0f, 0.0f, 1.0f, 1.0f },    // near
			 { 0.0f, 0.0f, 0.0f, 1.0f }     // far
			};

			XMVECTOR Determinant;
			XMMATRIX matInverse = XMMatrixInverse(&Determinant, projection);

			// Compute the frustum corners in world space.
			Vector4 Points[6];

			for (size_t i = 0; i < 6; ++i)
			{
				// Transform point.
				Points[i] = XMVector4Transform(HomogenousPoints[i], matInverse);
			}

			Out.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
			Out.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			// Compute the slopes.
			Points[0] = Points[0] * (Vector4)XMVectorReciprocal(XMVectorSplatZ(Points[0]));
			Points[1] = Points[1] * (Vector4)XMVectorReciprocal(XMVectorSplatZ(Points[1]));
			Points[2] = Points[2] * (Vector4)XMVectorReciprocal(XMVectorSplatZ(Points[2]));
			Points[3] = Points[3] * (Vector4)XMVectorReciprocal(XMVectorSplatZ(Points[3]));

			Out.RightSlope = XMVectorGetX(Points[0]);
			Out.LeftSlope = XMVectorGetX(Points[1]);
			Out.TopSlope = XMVectorGetY(Points[2]);
			Out.BottomSlope = XMVectorGetY(Points[3]);

			// Compute near and far.
			Points[4] = Points[4] * (Vector4)XMVectorReciprocal(XMVectorSplatW(Points[4]));
			Points[5] = Points[5] * (Vector4)XMVectorReciprocal(XMVectorSplatW(Points[5]));

			Out.Near = XMVectorGetZ(Points[4]);
			Out.Far = XMVectorGetZ(Points[5]);

			return Out;
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
