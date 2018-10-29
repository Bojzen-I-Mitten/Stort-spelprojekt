#pragma unmanaged
#pragma managed
#include "Viewport.h"

namespace ThomasEngine
{
	Viewport::Viewport(float x, float y, float w, float h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	Viewport::Viewport(Viewport% vp)
	{
		x = vp.x;
		y = vp.y;
		w = vp.w;
		h = vp.h;
	}
}