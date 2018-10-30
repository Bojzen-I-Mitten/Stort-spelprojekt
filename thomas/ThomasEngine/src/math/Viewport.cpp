#pragma unmanaged
#pragma managed
#include "Viewport.h"

namespace ThomasEngine
{
	Viewport::Viewport(float x, float y, float w, float h)
	{
		position = Vector2(x, y);
		size = Vector2(w, h);
	}
	Viewport::Viewport(Vector2 pos, Vector2 size)
	{
		this->position = pos;
		this->size = size;
	}
}