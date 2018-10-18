#pragma once


namespace ThomasEngine
{
	public ref class Random
	{
	public:
		static void InitState(int seed);

		static property Vector3 insideUnitSphere
		{
			Vector3 get();
		}

		static property Vector3 onUnitSphere
		{
			Vector3 get();
		}

		static float Range(float min, float max);
	};
}