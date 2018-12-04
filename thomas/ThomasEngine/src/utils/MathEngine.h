#pragma once


namespace ThomasEngine
{


	public ref class MathEngine
	{
	public:
		static Matrix CreateRotationMatrix(Vector3 right, Vector3 up, Vector3 back);
		/* Create rotation from orientation vectors
		*/
		static Quaternion CreateRotation(Vector3 right, Vector3 up, Vector3 backward);

		static Matrix CreateRotationXYZ(Vector3 angles);
		static Matrix CreateRotationXYZ(Vector3 right, Vector3 up, Vector3 backward, Vector3 angles);

		static float DegreesToRadians(float degree);
		static float RadiansToDegrees(float radians);
	};
}