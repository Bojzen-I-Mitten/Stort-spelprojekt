/*
	Singleton window management class
*/

#pragma once
#include <Windows.h>
#include <vector>
#include <d3d11.h>

namespace thomas 
{
	class Window
	{
	protected:
		struct DXBuffers
		{
			ID3D11RenderTargetView* backBuffer = nullptr;
			ID3D11ShaderResourceView* backBufferSRV = nullptr;
			ID3D11DepthStencilState* depthStencilState = nullptr;
			ID3D11DepthStencilView* depthStencilView = nullptr;
			ID3D11DepthStencilView* depthStencilViewReadOnly = nullptr;
			ID3D11ShaderResourceView* depthBufferSRV = nullptr;
		}m_dxBuffers;

	public:
		virtual ~Window();
		void QueueResize();
		void Bind();
		virtual void Present();
		void Clear();
		bool IsFocused();
		bool Initialized();
		bool ChangeWindowShowState(int nCmdShow);

	public:
		static void Destroy();
		static void ClearAllWindows();
		static void PresentAllWindows();
		static void Update();
		static void UpdateFocus();
		bool IntersectBounds(int x, int y) const;

	public:
		bool SetHeight(const LONG & height);
		bool SetWidth(const LONG & width);
		bool SetSize(const LONG & height, const LONG & width);
		void SetCursor(const bool & visible);

	public:
		static bool WaitingForUpdate();
		static LRESULT CALLBACK EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static Window* Create(HWND hWnd);

	public:
		static Window* GetWindow(int index);
		static Window* GetWindow(HWND hWnd);
		static Window* GetCurrentBound();
		static std::vector<Window*> GetWindows();

	public:
		LONG GetHeight() const;
		LONG GetWidth() const;
		RECT GetBounds() const;
		LONG GetHorizontalResolution() const;
		LONG GetVerticalResolution() const;
		IDXGISwapChain* GetSwapChain() const;
		HWND GetWindowHandler() const;
		float GetRealAspectRatio() const;

	protected:
		Window(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title);
	 	Window(HWND hWnd);

	protected:
		virtual void UpdateWindow();
		bool InitDxBuffers();
		bool Resize();

	protected:
		LONG m_width;
		LONG m_height;
		bool m_showCursor;
		bool m_fullScreen;
		bool m_initialized;
		bool m_shouldResize;
		bool m_created;
		bool m_focused;
		float m_aspectRatio;
		std::string m_title;

	protected:
		WNDCLASSEX m_windowClassInfo;
		HWND m_windowHandler;
		RECT m_windowRectangle;
		IDXGISwapChain* m_swapChain;

	protected:
		static std::vector<Window*> s_windows;
		static Window* s_current;
		static Window* s_focused;					// Window currently in focus
	};
}


