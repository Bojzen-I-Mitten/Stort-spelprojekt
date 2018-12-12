#include "Window.h"
#include "Common.h"
#include "WindowManager.h"
#include "ThomasCore.h"
#include "utils\AutoProfile.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace thomas 
{
	Window::Window(HWND& hwnd, HWND parent, LONG width, LONG height, std::string name) :
		m_shouldResize(false), m_title(name), m_showCursor(true), m_fullScreen(false), m_shouldSnappOutOfFullscreen(false), m_borderless(false),
		m_shouldStyleChange(false), m_hInstance(nullptr), m_initialized(false), m_input(Input()), m_windowStyle(0), m_width(width), m_height(height)
	{
		m_input.Init();
		m_hInstance = GetModuleHandle(NULL);

		m_windowClassInfo = { 0 };
		m_windowClassInfo.cbSize = sizeof(WNDCLASSEX);
		m_windowClassInfo.style = CS_HREDRAW | CS_VREDRAW;
		m_windowClassInfo.lpfnWndProc = EventHandler;
		m_windowClassInfo.hInstance = m_hInstance;
		m_windowClassInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
		m_windowClassInfo.lpszClassName = "ThomasWindow";
		m_windowClassInfo.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);

		RegisterClassEx(&m_windowClassInfo);
				
		/*m_windowRectangle = { 0, 0, width, height };
		AdjustWindowRect(&m_windowRectangle, m_windowStyle, FALSE);*/

		Create(hwnd, parent);
	}

	Window::~Window()
	{
		SAFE_RELEASE(m_dx.commandList);

		SAFE_RELEASE(m_dx.buffer[0]);
		SAFE_RELEASE(m_dx.buffer[1]);

		SAFE_RELEASE(m_dx.RTV[0]);
		SAFE_RELEASE(m_dx.RTV[1]);

		SAFE_RELEASE(m_dx.SRV[0]);
		SAFE_RELEASE(m_dx.SRV[1]);

		SAFE_RELEASE(m_dx.depthStencilState);

		SAFE_RELEASE(m_dx.depthStencilView[0]);
		SAFE_RELEASE(m_dx.depthStencilView[1]);

		SAFE_RELEASE(m_dx.depthStencilViewReadOnly[0]);
		SAFE_RELEASE(m_dx.depthStencilViewReadOnly[1]);

		SAFE_RELEASE(m_dx.depthBufferSRV);
		
		SAFE_RELEASE(m_swapChain);

		DestroyWindow(m_windowHandler);
	}	

	bool Window::Resize()
	{
		if (m_fullScreen)
		{
			m_width = 1920;
			m_height = 1080;

			SetWindowPos(m_windowHandler, nullptr, 0, 0, m_width, m_height, SWP_NOZORDER);
		}
		else if (m_shouldSnappOutOfFullscreen)
		{
			m_width = 800;
			m_height = 600;

			SetWindowPos(m_windowHandler, nullptr, 0, 0, m_width, m_height, SWP_NOZORDER | SWP_NOMOVE);
		}
		else
		{
			bool result = GetClientRect(m_windowHandler, &m_windowRectangle);
			if (result)
			{
				m_height = m_windowRectangle.bottom;
				m_width = m_windowRectangle.right;
			}
		}
		
		return ResizeBackBuffer();
	}

	bool Window::ResizeBackBuffer()
	{
		utils::D3D::Instance()->GetDeviceContextImmediate()->OMSetRenderTargets(0, NULL, NULL);

		SAFE_RELEASE(m_dx.commandList);

		SAFE_RELEASE(m_dx.buffer[0]);
		SAFE_RELEASE(m_dx.buffer[1]);

		SAFE_RELEASE(m_dx.RTV[0]);
		SAFE_RELEASE(m_dx.RTV[1]);

		SAFE_RELEASE(m_dx.SRV[0]);
		SAFE_RELEASE(m_dx.SRV[1]);

		SAFE_RELEASE(m_dx.depthStencilView[0]);
		SAFE_RELEASE(m_dx.depthStencilView[1]);

		SAFE_RELEASE(m_dx.depthStencilViewReadOnly[0]);
		SAFE_RELEASE(m_dx.depthStencilViewReadOnly[1]);

		SAFE_RELEASE(m_dx.depthBufferSRV);

		SAFE_RELEASE(m_dx.depthStencilState);

		utils::D3D::Instance()->GetDeviceContextImmediate()->ClearState();

		m_swapChain->ResizeBuffers(FRAME_BUFFERS, m_width, m_height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
		return InitDxBuffers();
	}

	bool Window::ChangeWindowStyle()
	{
		if (!m_borderless)
		{
			m_windowStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		}
		else
		{
			m_windowStyle = WS_POPUP;
		}

		SetWindowLong(m_windowHandler, GWL_STYLE, m_windowStyle);
		if (!SetWindowPos(m_windowHandler, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE))
			return false;
			
		return ResizeBackBuffer();
	}

	void Window::WaitOnSwapChain()
	{
		PROFILE("Wait-SwapChain")
		WaitForSingleObjectEx(m_waitableObject, 1000, true);
	}

	void Window::BeginFrame()
	{
		utils::D3D::Instance()->ResetCommandList(m_dx.commandList);
		Clear();
	}

	void Window::EndFrame()
	{
		utils::D3D::Instance()->FinishCommandList(m_dx.commandList);
		utils::D3D::Instance()->ExecuteCommandList(m_dx.commandList);
		Present();
	}

	void Window::UpdateWindow()
	{
		if (!m_initialized)
		{
			if (InitDxBuffers())
				m_initialized = true;
		}

		if (m_shouldResize)
		{
			if (Resize())
			{
				m_shouldResize = false;
				m_shouldSnappOutOfFullscreen = false;
			}
				
		}

		if (m_shouldStyleChange)
		{
			if (ChangeWindowStyle())
				m_shouldStyleChange = false;
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

	void Window::QueueWindowStyleChange()
	{
		m_shouldStyleChange = true;
	}

	bool Window::IntersectBounds(int x, int y) const
	{
		return m_windowRectangle.left <= x && x <= m_windowRectangle.right &&
			m_windowRectangle.top <= y && y <= m_windowRectangle.bottom;
	}

	void Window::BindBackBuffer()
	{
		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(0, NULL, NULL);

		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(1, &m_dx.RTV[0], m_dx.depthStencilView[0]);
		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetDepthStencilState(m_dx.depthStencilState, 1);
	}

	void Window::BindRenderTarget()
	{
		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(0, NULL, NULL);

		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetRenderTargets(1, &m_dx.RTV[1], m_dx.depthStencilView[1]);
		utils::D3D::Instance()->GetDeviceContextDeferred()->OMSetDepthStencilState(m_dx.depthStencilState, 1);
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
		bool hr = false;
		hr = utils::D3D::Instance()->CreateBackBuffer(m_swapChain, m_dx.buffer[0], m_dx.RTV[0], m_dx.SRV[0],
			m_dx.depthStencilView[0], m_dx.depthStencilViewReadOnly[0]);
			
		if (hr)
		{
			hr = utils::D3D::Instance()->CreateRenderTarget(m_dx.buffer[0], m_dx.buffer[1], m_dx.RTV[1], m_dx.SRV[1]);
			if (!hr)
			{
				LOG("Failed to create render target");
				return false;
			}

			hr = utils::D3D::Instance()->CreateDepthStencilView(m_dx.buffer[1], m_dx.depthStencilView[1], m_dx.depthStencilViewReadOnly[1], m_dx.depthBufferSRV);
			if (!hr)
			{
				LOG("Failed to create depth stencil view");
				return false;
			}

			m_dx.commandList = nullptr;
		}

		hr = utils::D3D::Instance()->CreateDepthStencilState(D3D11_COMPARISON_LESS, true, m_dx.depthStencilState);
		if (!hr)
		{
			LOG("Failed to create depth stencil state");
			return false;
		}

		return true;
	}

	bool Window::ShouldResize()
	{
		return m_shouldResize;
	}

	bool Window::ShouldWindowStyleChange()
	{
		return m_shouldStyleChange;
	}

	void Window::Clear()
	{
		float clearColor[] = { 0.34375f, 0.34375f, 0.34375f, 1.0f };

		utils::D3D::Instance()->GetDeviceContextDeferred()->ClearRenderTargetView(m_dx.RTV[1], clearColor);
		utils::D3D::Instance()->GetDeviceContextDeferred()->ClearDepthStencilView(m_dx.depthStencilView[1], D3D11_CLEAR_DEPTH, 1, 0);
		utils::D3D::Instance()->GetDeviceContextDeferred()->ClearDepthStencilView(m_dx.depthStencilView[0], D3D11_CLEAR_DEPTH, 1, 0);
	}

	void Window::Create(HWND& hwnd, HWND parent)
	{
		if (parent == nullptr)
		{
			if (!m_borderless)
				m_windowStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			else
				m_windowStyle = WS_POPUP;
		}
		else
		{
			m_windowStyle = WS_CHILD;
		}

		hwnd = CreateWindow(
			m_windowClassInfo.lpszClassName,
			m_title.c_str(),
			m_windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_width,
			m_height,
			parent,
			nullptr,
			m_hInstance,
			nullptr);

		if (hwnd)
		{
			m_windowHandler = hwnd;
			bool hr = CreateSwapChain();
			if (!hr)
				LOG("Failed to create swap chain");

			if (ChangeWindowShowState(SW_SHOW))
				LOG("Failed to change window state");
		}
		else
		{
			LOG("Failed to create window handler");
		}
	}

	bool Window::CreateSwapChain()
	{
		bool hr = utils::D3D::Instance()->CreateSwapChain(m_width, m_height, m_windowHandler, m_swapChain);
		if (hr)
		{
			m_waitableObject = m_swapChain->GetFrameLatencyWaitableObject();
			return true;
		}

		return false;
	}

	void Window::ResolveRenderTarget()
	{
		unsigned int sub = D3D11CalcSubresource(0, 0, 1);
		utils::D3D::Instance()->GetDeviceContextDeferred()->ResolveSubresource(m_dx.buffer[0], sub, m_dx.buffer[1], sub, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	void Window::SetCursor(const bool & visible)
	{
		if (m_showCursor != visible)
		{
			m_showCursor = visible;
			ShowCursor(visible);
		}
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		m_fullScreen = fullscreen;

		if (!m_fullScreen)
			m_shouldSnappOutOfFullscreen = true;
	}

	void Window::SetBorderless(bool borderless)
	{
		m_borderless = borderless;
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

	bool Window::GetFullScreen()
	{
		return m_fullScreen;
	}

	bool Window::GetBorderless()
	{
		return m_borderless;
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
			if (window)
				window->QueueResize();				
		break;
		case WM_ACTIVATEAPP:
			window->m_input.ProcessKeyboard(message, wParam, lParam);
			window->m_input.ProcessMouse(message, wParam, lParam, hWnd);
		break;
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
			SetFocus(hWnd);	// Set system focus on click (not to be confused with window hover focus)
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
			window->m_input.SetLastKey(NULL);
		break;
		case WM_CHAR:
			window->m_input.SetLastKey((unsigned short)wParam);
		break;
		case WM_STYLECHANGED:
			if (window)
				window->Create(hWnd, nullptr);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
