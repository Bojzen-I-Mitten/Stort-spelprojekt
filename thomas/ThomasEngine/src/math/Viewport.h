#pragma once
#pragma unmanaged
#pragma managed

namespace ThomasEngine
{
	public value class Viewport
	{
	public:
		Viewport(float x, float y, float w, float h);
		Viewport(Vector2 pos, Vector2 size);

		property Vector2 position;
		property Vector2 size;
	};
}