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

		using DirectX::BoundingBox;
		using DirectX::BoundingSphere;
		using DirectX::BoundingFrustum;
		using DirectX::BoundingOrientedBox;


		struct Euler {
			float yaw, pitch, roll;

			Euler() : yaw(0.f), pitch(0.f), roll(0.f) {}
			Euler(float yaw, float pitch, float roll) : yaw(yaw), pitch(pitch), roll(roll) {}
		};

		BoundingFrustum CreateFrustrumFromMatrixRH(CXMMATRIX projection);

		Quaternion getRotationTo(Vector3 from, Vector3 dest);

		Matrix getMatrixRotationTo(Vector3 from, Vector3 dest);

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