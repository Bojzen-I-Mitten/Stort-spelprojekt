#include "WindowManager.h"
#include "utils/GpuProfiler.h"

namespace thomas
{
	WindowManager WindowManager::s_windowManager;

	void WindowManager::Create(HWND& hwnd, HWND parent, int width, int height, bool isEditor)
	{
		if (isEditor)
		{
			m_editorWindow = new EditorWindow(hwnd, parent, width, height, "Editor");
			m_current = m_editorWindow;
			m_windows.push_back(m_editorWindow);
		}
		else
		{
			Window* window = new Window(hwnd, parent, width, height, "Game View");
			m_current = window;
			m_windows.push_back(window);
		}
	}

	void WindowManager::Create(HWND& hwnd, HWND parent, int width, int height, std::string name)
	{
		Window* window = new Window(hwnd, parent, width, height, name);
		m_current = window;
		m_windows.push_back(window);
	}

	void WindowManager::UpdateFocus()
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			HWND hWnd = WindowFromPoint(p);
			Window* window = GetWindow(hWnd);
			m_current = window;
		}
	}

	void WindowManager::SetBorderless(bool borderless)
	{
		if (m_current != nullptr && !m_current->IsEditor())
		{
			m_current->QueueWindowStyleChange();
			m_current->SetBorderless(borderless);
		}
	}

	void WindowManager::SetFullscreen(bool fullscreen)
	{
		if (m_current != nullptr && !m_current->IsEditor())
		{
			m_current->QueueResize();
			m_current->SetFullscreen(fullscreen);
		}
	}

	bool WindowManager::GetFullscreen()
	{
		if (m_current != nullptr && !m_current->IsEditor())
		{
			return m_current->GetFullScreen();
		}

		return false;
	}

	bool WindowManager::GetBorderless()
	{
		if (m_current != nullptr && !m_current->IsEditor())
		{
			return m_current->GetBorderless();
		}

		return false;
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

	void WindowManager::BeginFrame()
	{
		for (Window* window : m_windows)
			if (window->Initialized())
			{
				window->WaitOnSwapChain();
				window->BeginFrame();
			}

		utils::profiling::GpuProfiler::Instance()->RetriveTimeStamps();
		utils::profiling::GpuProfiler::Instance()->BeginFrame();
	}

	void WindowManager::EndFrame()
	{
		for (Window* window : m_windows)
			if (window->Initialized())
			{
				window->EndFrame();
			}

		utils::profiling::GpuProfiler::Instance()->EndFrame();
	}

	void WindowManager::ResolveRenderTarget()
	{
		for (Window* window : m_windows)
			if (window->Initialized())
			{
				window->ResolveRenderTarget();
			}
	}

	bool WindowManager::WaitingForUpdate()
	{
		for (Window* window : m_windows)
			if (window->ShouldResize() || window->ShouldWindowStyleChange())
				return true;

		return false;
	}

	void WindowManager::Update()
	{
		UpdateFocus();
		for (Window* window : m_windows)
		{
			window->UpdateWindow();
			if (m_current == window) {
				m_current->GetInput()->Update();
			}else
				window->GetInput()->Reset();
		}		
	}

	int WindowManager::GetNumOfWindows()
	{
		return (int)m_windows.size();
	}

	Window* WindowManager::GetCurrentBound()
	{
		return m_current;
	}

	Input * WindowManager::GetCurrentInput()
	{
		if (m_current)
			return m_current->GetInput();
		else
			return &m_dummyInput;
	}

	Input * WindowManager::GetGameInput()
	{
		if (!m_current || m_current->IsEditor())
			return &m_dummyInput;
		else
			return m_current->GetInput();
	}

	void WindowManager::SetCurrentBound(Window * value)
	{
		m_current = value;
	}

	EditorWindow * WindowManager::GetEditorWindow()
	{
		return m_editorWindow;
	}

	Window * WindowManager::GetWindow(int index)
	{
		if (m_editorWindow)
			index += 1; //offset to make -1 editor window.
		if ((index) < m_windows.size())
		{
			//s_current = m_windows[index + 1];
			return m_windows[index];
		}
			
		return nullptr;
	}

	Window * WindowManager::GetWindow(HWND hWnd)
	{
		for (auto window : m_windows)
			if (window->GetWindowHandler() == hWnd)
			{
				return window;
			}

		return nullptr;
	}
}