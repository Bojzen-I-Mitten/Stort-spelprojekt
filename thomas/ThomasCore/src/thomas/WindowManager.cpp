#include "WindowManager.h"

namespace thomas
{
	WindowManager WindowManager::s_windowManager;

	void WindowManager::Create(HWND hWnd, bool isEditor)
	{
		if (isEditor)
		{
			m_editorWindow = new EditorWindow(hWnd);
			s_current = m_editorWindow;
			m_windows.push_back(m_editorWindow);
		}
		else
		{
			Window* window = new Window(hWnd);
			s_current = window;
			m_windows.push_back(window);
		}
	}

	WindowManager* WindowManager::Instance()
	{
		return &s_windowManager;
	}

	void WindowManager::Destroy()
	{
		for (Window* window : m_windows)
			delete window;

		m_windows.clear();
	}

	void WindowManager::ClearAllWindows()
	{
		for (Window* window : m_windows)
			if (window->Initialized())
				window->Clear();
	}

	void WindowManager::PresentAllWindows()
	{
		for (Window* window : m_windows)
			if (window->Initialized())
			{
				window->Present();
				window->UnBind();
			}
	}

	bool WindowManager::WaitingForUpdate()
	{
		for (Window* window : m_windows)
			if (window->ShouldResize())
				return true;

		return false;
	}

	void WindowManager::Update()
	{
		for (Window* window : m_windows)
		{
			window->UpdateWindow();
			window->GetInput()->Update();
		}
	}

	int WindowManager::GetNumOfWindows()
	{
		return m_windows.size();
	}

	Window* WindowManager::GetCurrentBound()
	{
		return s_current;
	}

	EditorWindow * WindowManager::GetEditorWindow()
	{
		return m_editorWindow;
	}

	Window * WindowManager::GetWindow(int index)
	{
		if ((index + 1) < m_windows.size())
		{
			s_current = m_windows[index + 1];
			return m_windows[index + 1];
		}
			
		return nullptr;
	}

	Window * WindowManager::GetWindow(HWND hWnd)
	{
		for (auto window : m_windows)
			if (window->GetWindowHandler() == hWnd)
			{
				s_current = window;
				return window;
			}

		return nullptr;
	}
}