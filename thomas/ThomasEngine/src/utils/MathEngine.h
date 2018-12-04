#pragma once


namespace ThomasEngine
{


	public ref class MathEngine
	{
	public:
		/* Create rotation from orientation vectors
		*/
		static Quaternion CreateRotation(Vector3 right, Vector3 up, Vector3 forw);
	};
}