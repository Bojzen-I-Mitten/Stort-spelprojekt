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

	float Viewport::x::get() { return x; }
	void Viewport::x::set(float x) { this->x = x; }
	float Viewport::y::get() { return y; }
	void Viewport::y::set(float y) { this->y = y; }
	float Viewport::w::get() { return w; }
	void Viewport::w::set(float w) { this->w = w; }
	float Viewport::h::get() { return h; }
	void Viewport::h::set(float h) { this->h = h; }
}