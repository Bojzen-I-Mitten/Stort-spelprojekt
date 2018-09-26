
#include "Math.h"

namespace DirectX
{
	namespace SimpleMath
	{

		/** Gets the shortest arc quaternion to rotate this vector to the destination
			vector.
		@remarks
			If you call this with a dest vector that is close to the inverse
			of this vector, we will rotate 180 degrees around the 'fallbackAxis'
			(if specified, or a generated axis if not) since in this case
			ANY axis of rotation is valid.
		*/

		Quaternion getRotationTo(Vector3 from, Vector3 dest)
		{
			// Based on Stan Melax's article in Game Programming Gems
			Quaternion q;
			// Copy, since cannot modify local
			from.Normalize();
			dest.Normalize();

			float d = from.Dot(dest);
			// If dot == 1, vectors are the same
			if (d >= 1.0f || d < (1e-6f - 1.0f))
			{
				return Quaternion::Identity;
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