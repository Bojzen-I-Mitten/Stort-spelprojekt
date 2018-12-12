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
		bool GetBorderless();
		bool GetFullscreen();
	};
}
