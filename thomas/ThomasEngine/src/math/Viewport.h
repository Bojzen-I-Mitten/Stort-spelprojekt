#pragma once
#pragma unmanaged
#pragma managed

namespace ThomasEngine
{
	public ref class Viewport
	{
	public:
		Viewport(float x, float y, float w, float h);
		Viewport(Viewport% vp);

		float x;
		float y;
		float w;
		float h;
	};
}