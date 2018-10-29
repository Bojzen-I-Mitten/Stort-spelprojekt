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

		property float x
		{
			float get();
			void set(float x);
		}
		property float y
		{
			float get();
			void set(float y);
		}
		property float w
		{
			float get();
			void set(float w);
		}
		property float h
		{
			float get();
			void set(float h);
		}
	};
}