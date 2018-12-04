#include "MathEngine.h"



namespace ThomasEngine
{
	/* Create rotation from orientation vectors
	*/
	Quaternion MathEngine::CreateRotation(Vector3 right, Vector3 up, Vector3 forw)
	{
		Matrix m;
		m.M11 = right.x;	m.M12 = right.x;	m.M13 = right.x;	m.M14 = 0.f;
		m.M21 = up.x;		m.M22 = up.x;		m.M23 = up.x;		m.M24 = 0.f;
		m.M31 = forw.x;		m.M32 = forw.x;		m.M33 = forw.x;		m.M34 = 0.f;
		m.M41 = 0.f;		m.M42 = 0.f;		m.M43 = 0.f;		m.M44 = 1.f;
		return Quaternion::CreateFromRotationMatrix(m);
	}
}