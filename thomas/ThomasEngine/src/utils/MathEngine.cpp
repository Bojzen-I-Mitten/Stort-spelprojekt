#include "MathEngine.h"
#pragma unmanaged
#include<algorithm>
#include <thomas/utils/Math.h>
#pragma managed
using namespace System; // ::Math

namespace ThomasEngine

	/* Functions that need to be written properly...:
	*/
{
	/* Create rotation from orientation vectors
	*/
	Matrix MathEngine::CreateRotationMatrix(Vector3 right, Vector3 up, Vector3 back)
	{
		Matrix m;
		m.M11 = right.x;	m.M12 = right.y;	m.M13 = right.z;	m.M14 = 0.f;
		m.M21 = up.x;		m.M22 = up.y;		m.M23 = up.z;		m.M24 = 0.f;
		m.M31 = back.x;		m.M32 = back.y;		m.M33 = back.z;		m.M34 = 0.f;
		m.M41 = 0.f;		m.M42 = 0.f;		m.M43 = 0.f;		m.M44 = 1.f;
		return m;
	}
	/* Create rotation from orientation vectors
	*/
	Quaternion MathEngine::CreateRotation(Vector3 right, Vector3 up, Vector3 back)
	{
		Matrix m;
		m.M11 = right.x;	m.M12 = right.y;	m.M13 = right.z;	m.M14 = 0.f;
		m.M21 = up.x;		m.M22 = up.y;		m.M23 = up.z;		m.M24 = 0.f;
		m.M31 = back.x;		m.M32 = back.y;		m.M33 = back.z;		m.M34 = 0.f;
		m.M41 = 0.f;		m.M42 = 0.f;		m.M43 = 0.f;		m.M44 = 1.f;
		return Quaternion::CreateFromRotationMatrix(m);
	}
	Matrix MathEngine::CreateRotationXYZ(Vector3 angles)
	{
		return Matrix::CreateRotationX(DegreesToRadians(angles.x)) * 
			Matrix::CreateRotationY(DegreesToRadians(angles.y)) * 
			Matrix::CreateRotationZ(DegreesToRadians(angles.z));
	}
	Matrix MathEngine::CreateRotationXYZ(Vector3 right, Vector3 up, Vector3 back, Vector3  angles)
	{
		return Matrix::CreateFromAxisAngle(right, DegreesToRadians(angles.x)) *
			Matrix::CreateFromAxisAngle(up, DegreesToRadians(angles.y)) *
			Matrix::CreateFromAxisAngle(back, DegreesToRadians(angles.z));
	}
	float MathEngine::DegreesToRadians(float degree)
	{
		return thomas::math::DegreesToRadians(degree);
	}
	float MathEngine::RadiansToDegrees(float radians)
	{
		return thomas::math::RadiansToDegrees(radians);
	}
}