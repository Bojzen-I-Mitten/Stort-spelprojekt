#pragma once

#pragma unmanaged

#pragma managed
#include "WindowManager.h"
#include "ThomasManaged.h"

namespace ThomasEngine
{
	WindowManager::WindowManager()
	{
	}

	void WindowManager::SetBorderless(bool borderless)
	{
		if (!ThomasWrapper::IsEditorBuild())
			thomas::WindowManager::Instance()->SetBorderless(borderless);
	}

	void WindowManager::SetFullscreen(bool Fullscreen)
	{
		if (!ThomasWrapper::IsEditorBuild())
			thomas::WindowManager::Instance()->SetFullscreen(Fullscreen);
	}

	void WindowManager::SetWidth(LONG width)
	{
		if (!ThomasWrapper::IsEditorBuild())
			thomas::WindowManager::Instance()->SetWidth(width);
	}

	void WindowManager::SetHeight(LONG height)
	{
		if (!ThomasWrapper::IsEditorBuild())
			thomas::WindowManager::Instance()->SetHeight(height);
	}

	bool WindowManager::GetBorderless()
	{
		if (!ThomasWrapper::IsEditorBuild())
			return thomas::WindowManager::Instance()->GetBorderless();

		return false;
	}

	bool WindowManager::GetFullscreen()
	{
		if (!ThomasWrapper::IsEditorBuild())
			return thomas::WindowManager::Instance()->GetFullscreen();

		return false;
	}

	LONG WindowManager::GetWidth()
	{
		if (!ThomasWrapper::IsEditorBuild())
			return thomas::WindowManager::Instance()->GetWidth();

		return 0;
	}

	LONG WindowManager::GetHeight()
	{
		if (!ThomasWrapper::IsEditorBuild())
			return thomas::WindowManager::Instance()->GetHeight();

		return 0;
	}
}