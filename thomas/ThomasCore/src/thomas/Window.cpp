#include "Window.h"
#include "Common.h"
#include "WindowManager.h"
#include "ThomasCore.h"
#include "../thomas/utils/AutoProfile.h"

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
				m_waitableObject = m_swapChain->GetFrameLatencyWaitableObject();
				m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
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

			bool hr = utils::D3D::Instance()->CreateSwapChain(m_width, m_height, m_windowHandler, m_swapChain);
			if (hr)
			{
				m_waitableObject = m_swapChain->GetFrameLatencyWaitableObject();
				m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
			}
		}
		else
			LOG("Failed to create window");
	}

	Window::~Window()
	{
		for (int i = 0; i < FRAME_BUFFERS; i++)
		{
			SAFE_RELEASE(m_dx.commandList[i]);

			SAFE_RELEASE(m_dx.buffer[0][i]);
			SAFE_RELEASE(m_dx.buffer[1][i]);

			SAFE_RELEASE(m_dx.RTV[0][i]);
			SAFE_RELEASE(m_dx.RTV[1][i]);

			SAFE_RELEASE(m_dx.SRV[0][i]);
			SAFE_RELEASE(m_dx.SRV[1][i]);

			SAFE_RELEASE(m_dx.depthStencilState);

			SAFE_RELEASE(m_dx.depthStencilView[0][i]);
			SAFE_RELEASE(m_dx.depthStencilView[1][i]);

			SAFE_RELEASE(m_dx.depthStencilViewReadOnly[0][i]);
			SAFE_RELEASE(m_dx.depthStencilViewReadOnly[1][i]);

			SAFE_RELEASE(m_dx.depthBufferSRV[i]);
		}

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

			utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetRenderTargets(0, 0, 0);
			utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetDepthStencilState(NULL, 1);

			for (int i = 0; i < FRAME_BUFFERS; i++)
			{
				SAFE_RELEASE(m_dx.commandList[i]);

				SAFE_RELEASE(m_dx.buffer[0][i]);
				SAFE_RELEASE(m_dx.buffer[1][i]);

				SAFE_RELEASE(m_dx.RTV[0][i]);
				SAFE_RELEASE(m_dx.RTV[1][i]);

				SAFE_RELEASE(m_dx.SRV[0][i]);
				SAFE_RELEASE(m_dx.SRV[1][i]);

				SAFE_RELEASE(m_dx.depthStencilView[0][i]);
				SAFE_RELEASE(m_dx.depthStencilView[1][i]);

				SAFE_RELEASE(m_dx.depthStencilViewReadOnly[0][i]);
				SAFE_RELEASE(m_dx.depthStencilViewReadOnly[1][i]);

				SAFE_RELEASE(m_dx.depthBufferSRV[i]);
			}

			SAFE_RELEASE(m_dx.depthStencilState);

			m_swapChain->ResizeBuffers(FRAME_BUFFERS, m_width, m_height, DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
			return InitDxBuffers();
		}
		else
			return false;
	}

	void Window::WaitOnSwapChain()
	{
		PROFILE("WAITGPU")
		DWORD result = WaitForSingleObjectEx(m_waitableObject, 1000, true);
	}

	void Window::BeginFrame()
	{
		utils::D3D::Instance()->ResetCommandList(m_dx.commandList[m_frameIndex]);
		Clear();
	}

	void Window::EndFrame()
	{
		utils::D3D::Instance()->FinishCommandList(m_dx.commandList[m_frameIndex]);
		utils::D3D::Instance()->ExecuteCommandList(m_dx.commandList[m_frameIndex]);
		Present();
		
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
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

	void Window::BindBackBuffer()
	{
		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetRenderTargets(0, 0, 0);

		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetRenderTargets(1, &m_dx.RTV[0][m_frameIndex], m_dx.depthStencilView[0][m_frameIndex]);
		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetDepthStencilState(m_dx.depthStencilState, 1);
	}

	void Window::BindRenderTarget()
	{
		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetRenderTargets(0, 0, 0);

		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetRenderTargets(1, &m_dx.RTV[1][m_frameIndex], m_dx.depthStencilView[1][m_frameIndex]);
		utils::D3D::Instance()->GetDeviceContextDeffered()->OMSetDepthStencilState(m_dx.depthStencilState, 1);
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
		for (int i = 0; i < FRAME_BUFFERS; i++)
		{
			hr = utils::D3D::Instance()->CreateBackBuffer(m_swapChain, m_dx.buffer[0][i], m_dx.RTV[0][i], m_dx.SRV[0][i],
				m_dx.depthStencilView[0][i], m_dx.depthStencilViewReadOnly[0][i]);
			
			if (hr)
			{
				hr = utils::D3D::Instance()->CreateRenderTarget(m_dx.buffer[0][i], m_dx.buffer[1][i], m_dx.RTV[1][i], m_dx.SRV[1][i]);
				if (!hr)
				{
					LOG("Failed to create render target");
					return false;
				}

				hr = utils::D3D::Instance()->CreateDepthStencilView(m_dx.buffer[1][i], m_dx.depthStencilView[1][i], m_dx.depthStencilViewReadOnly[1][i], m_dx.depthBufferSRV[i]);
				if (!hr)
				{
					LOG("Failed to create depth stencil view");
					return false;
				}

				m_dx.commandList[i] = nullptr;
			}
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

	void Window::Clear()
	{
		float clearColor[] = { 0.34375f, 0.34375f, 0.34375f, 1.0f };

		utils::D3D::Instance()->GetDeviceContextDeffered()->ClearRenderTargetView(m_dx.RTV[1][m_frameIndex], clearColor);
		utils::D3D::Instance()->GetDeviceContextDeffered()->ClearDepthStencilView(m_dx.depthStencilView[1][m_frameIndex], D3D11_CLEAR_DEPTH, 1, 0);
	}

	void Window::ResolveRenderTarget()
	{
		unsigned int sub = D3D11CalcSubresource(0, 0, 1);
		utils::D3D::Instance()->GetDeviceContextDeffered()->ResolveSubresource(m_dx.buffer[0][m_frameIndex], sub, m_dx.buffer[1][m_frameIndex], sub, DXGI_FORMAT_R8G8B8A8_UNORM);
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
