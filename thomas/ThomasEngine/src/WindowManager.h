#pragma once
#pragma unmanaged
#include <thomas/WindowManager.h>

#pragma managed

namespace ThomasEngine
{
	public ref class WindowManager
	{
	public:
		WindowManager();
		void SetBorderless(bool borderless);
		void SetFullscreen(bool Fullscreen);
		void SetWidth(LONG width);
		void SetHeight(LONG height);
		bool GetBorderless();
		bool GetFullscreen();
		LONG GetWidth();
		LONG GetHeight();
	};
}
