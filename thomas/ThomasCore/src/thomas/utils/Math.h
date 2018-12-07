#pragma once
#pragma warning(disable:4561)

#include <d3d11.h>
#include "DirectXTK\SimpleMath.h"
#include <cmath>
#include <string>


namespace DirectX
{
	namespace SimpleMath
	{
		constexpr float PI = 3.141592654f;
		constexpr float EPSILON = 1e-5f;

		using DirectX::BoundingBox;
		using DirectX::BoundingSphere;
		using DirectX::BoundingFrustum;
		using DirectX::BoundingOrientedBox;


		struct Euler {
			float yaw, pitch, roll;

			Euler() : yaw(0.f), pitch(0.f), roll(0.f) {}
			Euler(float yaw, float pitch, float roll) : yaw(yaw), pitch(pitch), roll(roll) {}
		};
		enum Axis {
			AxisX,
			AxisY,
			AxisZ,
			NegAxisX,
			NegAxisY,
			NegAxisZ
		};

		/*	Extract the specified axis.
		*/
		inline Vector3 getAxis(Matrix& objectPose, Axis ind)
		{
			uint32_t i = ind % 3;
			return Vector3(objectPose(i,0), objectPose(i, 1), objectPose(i, 2)) * (ind > 2 ? -1.f : 1.f);
		}
		/*	Find orthogonal axis to the vector
		*/
		inline Vector3 orthogonal(const Vector3& vec)
		{
			// Orthogonal vector: <v, v_o> = 0
			// Algortithm by Ahmed F: https://math.stackexchange.com/questions/133177/finding-a-unit-vector-perpendicular-to-another-vector
			// Find first v_m != 0 and return:
			// (0,.., v_(m+1), -v_m, 0, 0, 0) 
			// Which gives: 
			// v_m * v_(m+1) + v_(m+1)*-v_m = 0
			// 
			// Only check first x if 0, otherwise no particular diff. in swizzling y->z to z->x when x=y=0 
			// (exception when epsilon's magnitude matters)
			return std::fabs(vec.x) < EPSILON ?
				Vector3(vec.y, -vec.x, 0.f) :
				Vector3(0.f, vec.z, -vec.y);
		}


		BoundingFrustum CreateFrustrumFromMatrixRH(CXMMATRIX projection);

		Quaternion getRotationTo(Vector3 from, Vector3 dest);

		Matrix getMatrixRotationTo(Vector3 from, Vector3 dest);
		Matrix getMatrixRotationTo_Nor(Vector3 from, Vector3 dest);


		/* Extract the length of each axis in the top-left 3x3 matrix.
		*/
		Vector3 extractAxisScale(const Matrix& m);
		/* Normalize each row vector in the topleft 3x3 matrix
		*/
		Matrix normalizeBasisAxis(Matrix m);
		Matrix extractRotation(Matrix m);
		XMFLOAT3X3 extractRotation3x3(const Matrix &m);
		/* Multiply first three axis by each component. Equivalent to m * row_vec4(scalars, 1)
		*/
		Matrix& mult(Matrix &m, Vector3 scalars);

		inline Vector3 Normalize(Vector3 v)
		{
			v.Normalize();
			return v;
		}
		inline Vector3 lerp(const Vector3& from, const Vector3& to, float amount)
		{
			return from * (1.f - amount) + to * amount;
		}


		//phi [0, 2*pi], theta [0, pi]
		Vector3 SphericalCoordinate(float phi, float theta, float radius = 1);

		inline float DegreesToRadians(const float & degree)
		{
			return std::fmodf(degree, 360.f) * (PI / 180.f);
		}

		inline float RadiansToDegrees(const float & radian)
		{
			return radian * (180.f / PI);
		}

		inline float Lerp(const float & from, const float & to, const float & amount)
		{
			return from + (to - from) * amount;
		}

		inline float square(float x) {
			return x * x;
		}
		/* Clamp within the range
		*/ 
		template<typename N>
		inline N clamp(N v, N minimum, N maximum)
		{
			return min(max(v, minimum), maximum);
		}

		/* Convert rotation quatenion to pitch/yaw/roll vector
		*/
		Euler ToEuler(const Quaternion & q);
		Euler ToEuler(const Vector3 &v);
		Vector3 FromEuler(const Euler &e);

		/* Convert pitch/yaw/roll vector to rotation quaternion
		*/
		inline Quaternion FromEuler(const Vector3 & rotation);

		inline std::string ToString(const Vector3 & v)
		{
			return "(" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + ")";
		}

		inline Matrix CreateMatrix(const Vector3 & position, const Quaternion & rotation, const Vector3 & scale)
		{
			Matrix rot = Matrix::CreateFromQuaternion(rotation);
			return Matrix::CreateScale(scale) * Matrix::CreateWorld(position, rot.Forward(), rot.Up());
		}
	}
}

namespace thomas
{
	namespace math = DirectX::SimpleMath;
}