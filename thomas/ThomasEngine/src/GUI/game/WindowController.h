#pragma once
#pragma unmanaged
#pragma managed
#include "../../../../ThomasCore/src/thomas/WindowManager.h"
namespace thomas { 
	class Window; 
}
namespace ThomasEngine
{
	public ref class WindowController
	{
	public :
		WindowController();
		void SetBorderless(bool borderless);
		void SetFullscreen(bool Fullscreen);
	internal:
		thomas::WindowManager* nativePtr;


	};
}