#include "Window.h"
#include "Common.h"
#include "utils\D3D.h"
#include "WindowManager.h"
#include "ThomasCore.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace thomas 
{
	Window::Window(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title) :
		m_shouldResize(false), m_width(width), m_height(height), m_title(std::string(title)), m_showCursor(false), m_fullScreen(false)
	{
		m_windowClassInfo = { 0 };
		m_windowClassInfo.cbSize = sizeof(WNDCLASSEX);
		m_windowClassInfo.style = CS_HREDRAW | CS_VREDRAW;
		m_windowClassInfo.lpfnWndProc = EventHandler; //Callback for EVENTS
		m_windowClassInfo.hInstance = hInstance;
		m_windowClassInfo.hInstance = hInstance;
		m_windowClassInfo.lpszClassName = "ThomasWindow";
		m_windowClassInfo.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

		if (!RegisterClassEx(&m_windowClassInfo))
			LOG("Failed to create window")

		m_windowRectangle = { 0, 0, m_width, m_height };
		//Properties for window
		AdjustWindowRect(&m_windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
		m_windowHandler = CreateWindow(
			"ThomasEngine",
			m_title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_windowRectangle.right - m_windowRectangle.left,
			m_windowRectangle.bottom - m_windowRectangle.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr);

		m_initialized = false;
		if (m_windowHandler)
		{
			bool hr = utils::D3D::Instance()->CreateSwapChain(m_width, m_height, m_windowHandler, m_swapChain);
			if (hr)
			{
				ChangeWindowShowState(nCmdShow);
			}
		}
	}

	Window::Window(HWND hWnd) : m_shouldResize(false), m_initialized(false), m_input(Input())
	{
		m_input.Init();

		bool result = GetWindowRect(hWnd, &m_windowRectangle);
		if (result)
		{
			m_height = m_windowRectangle.bottom > 0 ? m_windowRectangle.bottom : 10;
			m_width = m_windowRectangle.right > 0 ? m_windowRectangle.right : 10;
			m_showCursor = true; m_fullScreen = false;
			m_windowHandler = hWnd;

			utils::D3D::Instance()->CreateSwapChain(m_width, m_height, m_windowHandler, m_swapChain);
		}
		else
			LOG("Failed to create window");
	}

	Window::~Window()
	{
		SAFE_RELEASE(m_dxBuffers.backBuffer);
		SAFE_RELEASE(m_dxBuffers.backBufferSRV);
		SAFE_RELEASE(m_dxBuffers.depthStencilState);
		SAFE_RELEASE(m_dxBuffers.depthStencilView);
		SAFE_RELEASE(m_dxBuffers.depthStencilViewReadOnly);
		SAFE_RELEASE(m_dxBuffers.depthBufferSRV);
		SAFE_RELEASE(m_swapChain);

		DestroyWindow(m_windowHandler);
	}	

	bool Window::Resize()
	{
		bool result = GetWindowRect(m_windowHandler, &m_windowRectangle);
		if (result)
		{
			LONG newWidth = m_windowRectangle.right - m_windowRectangle.left;
			LONG newHeight = m_windowRectangle.bottom - m_windowRectangle.top;

			if (m_height == newHeight && m_width == newWidth)
				return false;
			m_height = newHeight;
			m_width = newWidth;

			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(0, 0, 0);
			utils::D3D::Instance()->GetDeviceContext()->OMSetDepthStencilState(NULL, 1);
			SAFE_RELEASE(m_dxBuffers.backBuffer);
			SAFE_RELEASE(m_dxBuffers.backBufferSRV);
			SAFE_RELEASE(m_dxBuffers.depthStencilState);
			SAFE_RELEASE(m_dxBuffers.depthStencilView);
			SAFE_RELEASE(m_dxBuffers.depthStencilViewReadOnly);
			SAFE_RELEASE(m_dxBuffers.depthBufferSRV);

			m_swapChain->ResizeBuffers(0, m_width, m_height, DXGI_FORMAT_UNKNOWN, 0);
			return InitDxBuffers();
		}
		else
			return false;
	}

	void Window::UpdateWindow()
	{
		if (!m_initialized)
		{
			InitDxBuffers();
			m_initialized = true;
		}
		if (m_shouldResize)
		{
			Resize();
			m_shouldResize = false;
		}
	}

	bool Window::Initialized()
	{
		return m_initialized;
	}

	void Window::QueueResize()
	{
		m_shouldResize = true;
	}

	bool Window::IntersectBounds(int x, int y) const
	{
		return m_windowRectangle.left <= x && x <= m_windowRectangle.right &&
			m_windowRectangle.top <= y && y <= m_windowRectangle.bottom;
	}

	void Window::Bind()
	{
			utils::D3D::Instance()->GetDeviceContext()->OMSetRenderTargets(1, &m_dxBuffers.backBuffer, m_dxBuffers.depthStencilView);
			utils::D3D::Instance()->GetDeviceContext()->OMSetDepthStencilState(m_dxBuffers.depthStencilState, 1);
	}


	bool Window::ChangeWindowShowState(int nCmdShow)
	{
		return ShowWindow(m_windowHandler, nCmdShow);
	}

	void Window::Present()
	{
		m_swapChain->Present(0, 0);
	}

	bool Window::InitDxBuffers()
	{
		bool hr = utils::D3D::Instance()->CreateBackBuffer(m_swapChain, m_dxBuffers.backBuffer, m_dxBuffers.backBufferSRV);
		if (hr)
		{
			hr = utils::D3D::Instance()->CreateDepthStencilView(m_width, m_height, m_dxBuffers.depthStencilView, m_dxBuffers.depthStencilViewReadOnly, m_dxBuffers.depthBufferSRV);
			if (hr)
			{
				hr = m_dxBuffers.depthStencilState = utils::D3D::Instance()->CreateDepthStencilState(D3D11_COMPARISON_LESS, true);
				if (hr)
					return true;
			}
		}
		LOG("Failed to create DirectX window buffers");
		return false;	
	}

	bool Window::ShouldResize()
	{
		return m_shouldResize;
	}

	void Window::Clear()
	{
		float clearColor[] = { 0.34375f, 0.34375f, 0.34375f, 1.0f };
		utils::D3D::Instance()->GetDeviceContext()->ClearRenderTargetView(m_dxBuffers.backBuffer, clearColor);
		utils::D3D::Instance()->GetDeviceContext()->ClearDepthStencilView(m_dxBuffers.depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	}

	void Window::SetCursor(const bool & visible)
	{
		if (m_showCursor != visible)
		{
			m_showCursor = visible;
			ShowCursor(visible);
		}
	}

	bool Window::SetHeight(const LONG & height)
	{
		if (height > 0 && height <= GetVerticalResolution())
		{
			m_height = height;
			return true;
		}

		return false;
	}

	bool Window::SetWidth(const LONG & width)
	{
		if (width > 0 && width <= GetHorizontalResolution())
		{
			m_width = width;
			return true;
		}

		return false;
	}

	bool Window::SetSize(const LONG & height, const LONG & width)
	{
		if (SetWidth(width) && SetHeight(height))
			return true;

		return false;
	}

	IDXGISwapChain * Window::GetSwapChain() const
	{
		return m_swapChain;
	}

	Input* Window::GetInput()
	{
		return &m_input;
	}

	LONG Window::GetHeight() const
	{
		return m_height;
	}

	LONG Window::GetWidth() const
	{
		return m_width;
	}

	RECT Window::GetBounds() const
	{
		return m_windowRectangle;
	}

	float Window::GetRealAspectRatio() const
	{
		return (float)m_width / (float)m_height;
	}

	HWND Window::GetWindowHandler() const
	{
		return m_windowHandler;
	}

	LONG Window::GetHorizontalResolution() const
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		return desktop.right;
	}

	LONG Window::GetVerticalResolution() const
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		return desktop.bottom;
	}


	//Windows window events function
	LRESULT CALLBACK Window::EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
			return true;

		Window* window = WindowManager::Instance()->GetWindow(hWnd);
		if (!window)
			return DefWindowProc(hWnd, message, wParam, lParam);
		//If one case is hit the code will execute everything down until a break;
		switch (message)
		{
		case WM_SIZE:
		{
			if (window)
				window->QueueResize();				
		}
		break;
		case WM_SETFOCUS:
			break;
		case WM_KILLFOCUS:
			break;
		case WM_ACTIVATEAPP:
				window->m_input.ProcessKeyboard(message, wParam, lParam);
				window->m_input.ProcessMouse(message, wParam, lParam, hWnd);
			break;
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
			SetFocus(hWnd);	// Set system focus on click (not to be confused with window hower focus)
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			window->m_input.ProcessMouse(message, wParam, lParam, hWnd);
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			window->m_input.ProcessKeyboard(message, wParam, lParam);
			break;
		case WM_CHAR:
			if (wParam > 0 && wParam < 0x10000)
				window->m_input.SetLastKey((unsigned short)wParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
