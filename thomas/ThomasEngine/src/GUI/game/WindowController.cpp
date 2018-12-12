#include "WindowController.h"

ThomasEngine::WindowController::WindowController()
{
	nativePtr = thomas::WindowManager::Instance();
}

void ThomasEngine::WindowController::SetBorderless(bool borderless)
{
	nativePtr->setBorderless(borderless);
}

void ThomasEngine::WindowController::SetFullscreen(bool Fullscreen)
{
	nativePtr->setFullscreen(Fullscreen);
}

bool ThomasEngine::WindowController::getBorderless()
{
	return nativePtr->getBorderless();
}

bool ThomasEngine::WindowController::getFullscreen()
{
	return nativePtr->getFullscreen();
}
