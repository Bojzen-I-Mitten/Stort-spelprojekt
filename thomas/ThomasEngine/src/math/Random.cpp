
#pragma unmanaged
#include <stdlib.h>     /* srand, rand */
#include <math.h>
#pragma managed

#include "Random.h"

namespace ThomasEngine
{
	void Random::InitState(int seed)
	{
		srand(seed);
	}

	float Random::Range(float min, float max)
	{
		return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	}

	Vector3 Random::insideUnitSphere::get() 
	{
		float radius = ((float)rand() / (RAND_MAX)); // 0 < radius < 1
		return  radius * onUnitSphere;
	}

	Vector3 Random::onUnitSphere::get() 
	{
		float theta = Range(0.0f, MathHelper::Pi);
		float phi = Range(0.0f, MathHelper::TwoPi);
		Vector3 sphere;
		sphere.x = sinf(theta) * cosf(phi);
		sphere.y = sinf(theta) * sinf(phi);
		sphere.z = cosf(theta);
		return sphere;
	}

}