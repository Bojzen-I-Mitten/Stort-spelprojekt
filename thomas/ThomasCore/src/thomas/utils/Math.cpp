#include "Math.h"


namespace DirectX
{
	namespace SimpleMath
	{
#define assertNormal(v) assert(std::fabs(v.Length() - 1.f) < EPSILON)

		Euler ToEuler(const Quaternion & q)
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

			return Euler(RadiansToDegrees(yaw), RadiansToDegrees(pitch),  RadiansToDegrees(roll));
		}
		Euler ToEuler(const Vector3 & v)
		{
			return { v.y, v.x, v.z };
		}
		Vector3 FromEuler(const Euler & e)
		{
			return Vector3(e.pitch, e.yaw, e.roll);
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

		Vector3 SphericalCoordinate(float phi, float theta, float radius)
		{
			float xAngle = sin(theta) * cos(phi);
			float yAngle = cos(theta);
			float zAngle = sin(theta) * sin(phi); 

			return Vector3(xAngle, yAngle, zAngle) * radius;
		}

		Matrix getMatrixRotationTo(Vector3 from, Vector3 dest)
		{
			from.Normalize();
			dest.Normalize();
			return getMatrixRotationTo_Nor(from, dest);
		}
		Matrix getMatrixRotationTo_Nor(Vector3 from, Vector3 dest)
		{
			assertNormal(from);
			assertNormal(dest);
			float c = from.Dot(dest);
			if (c >= 1.0f) // Vectors are parallel	(identical)
				return Matrix::Identity;
			if (c < (1e-6f - 1.0f)) // Vectors are parallel (opposite)
			{
				// Generate an axis
				Vector3 axis = Vector3::UnitX.Cross(from);
				if (axis.LengthSquared() < EPSILON) // Pick another if colinear
					axis = Vector3::UnitY.Cross(from);
				axis.Normalize();
				return Matrix::CreateFromAxisAngle(axis, PI);
			}
			else {

				// Credit goes to: Jur Van den Berg 
				// https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d/476311#476311
				Vector3 v = from.Cross(dest);
				float s = 1.f / (1.f + c);

				Matrix vx(
					0.f, v.z, -v.y, 0.f,
					-v.z, 0.f, v.x, 0.f,
					v.y, -v.x, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f);
				vx = vx + (vx * vx) * s; // Simplify square calc? ...
				// Add identity
				vx._11 += 1.f;
				vx._22 += 1.f;
				vx._33 += 1.f;
				vx._44 += 1.f;
				return vx;
			}
		}


		/* Extract the length of each axis in the top-left 3x3 matrix.
		*/
		Vector3 extractAxisScale(const Matrix& m)
		{
			return Vector3(
				m.Right().Length(),
				m.Up().Length(),
				m.Forward().Length());
		}
		/* Normalize each row vector in the topleft 3x3 matrix
		*/
		Matrix normalizeBasisAxis(Matrix m)
		{
			float len = 1.f / std::sqrtf(m._11 * m._11 + m._12 * m._12 + m._13 * m._13);
			m._11 *= len;
			m._12 *= len;
			m._13 *= len;
			len = 1.f / std::sqrtf(m._21 * m._21 + m._22 * m._22 + m._23 * m._23);
			m._21 *= len;
			m._22 *= len;
			m._23 *= len;
			len = 1.f / std::sqrtf(m._31 * m._31 + m._32 * m._32 + m._33 * m._33);
			m._31 *= len;
			m._32 *= len;
			m._33 *= len;
			return m;
		}
		Matrix extractRotation(Matrix m)
		{
			float len = 1.f / std::sqrtf(m._11 * m._11 + m._12 * m._12 + m._13 * m._13);
			m._11 *= len;
			m._12 *= len;
			m._13 *= len;
			m._14 = 0;
			len = 1.f / std::sqrtf(m._21 * m._21 + m._22 * m._22 + m._23 * m._23);
			m._21 *= len;
			m._22 *= len;
			m._23 *= len;
			m._24 = 0;
			len = 1.f / std::sqrtf(m._31 * m._31 + m._32 * m._32 + m._33 * m._33);
			m._31 *= len;
			m._32 *= len;
			m._33 *= len;
			m._34 = 0;
			m._41 = 0;
			m._42 = 0;
			m._43 = 0;
			m._44 = 1;
			return m;
		}
		XMFLOAT3X3 extractRotation3x3(const Matrix & src)
		{
			XMFLOAT3X3 m;
			float len = 1.f / std::sqrtf(src._11 * src._11 + src._12 * src._12 + src._13 * src._13);
			m._11 = src._11 * len;
			m._12 = src._12 * len;
			m._13 = src._13 * len;
			len = 1.f / std::sqrtf(src._21 * src._21 + src._22 * src._22 + src._23 * src._23);
			m._21 = src._21 * len;
			m._22 = src._22 * len;
			m._23 = src._23 * len;
			len = 1.f / std::sqrtf(src._31 * src._31 + src._32 * src._32 + src._33 * src._33);
			m._31 = src._31 * len;
			m._32 = src._32 * len;
			m._33 = src._33 * len;
			return m;
		}
		/* Multiply first three axis by each component. Equivalent to m * row_vec4(scalars, 1)
		*/
		Matrix& mult(Matrix &m, Vector3 scalars)
		{
			m._11 *= scalars.x;
			m._12 *= scalars.x;
			m._13 *= scalars.x;
			m._21 *= scalars.y;
			m._22 *= scalars.y;
			m._23 *= scalars.y;
			m._31 *= scalars.z;
			m._32 *= scalars.z;
			m._33 *= scalars.z;
			return m;
		}
	}
}
