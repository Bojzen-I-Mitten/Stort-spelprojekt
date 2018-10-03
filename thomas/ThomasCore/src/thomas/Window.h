#pragma once
#include <Windows.h>
#include <vector>
#include <d3d11.h>
#include "Input.h"

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
		Window(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title);
		Window(HWND hWnd);
		virtual ~Window();

	public:
		virtual void UpdateWindow();
		virtual void Present();
		void QueueResize();
		void Bind();
		void UnBind();
		void Clear();

	public:
		bool ShouldResize();
		bool Initialized();
		bool ChangeWindowShowState(int nCmdShow);
		bool IntersectBounds(int x, int y) const;

	public:
		bool SetHeight(const LONG & height);
		bool SetWidth(const LONG & width);
		bool SetSize(const LONG & height, const LONG & width);
		void SetCursor(const bool & visible);

	public:
		static LRESULT CALLBACK EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		Input* GetInput();
		LONG GetHeight() const;
		LONG GetWidth() const;
		RECT GetBounds() const;
		LONG GetHorizontalResolution() const;
		LONG GetVerticalResolution() const;
		Window* GetCurrentBound();
		IDXGISwapChain* GetSwapChain() const;
		HWND GetWindowHandler() const;
		float GetRealAspectRatio() const;

	protected:
		bool InitDxBuffers();
		bool Resize();

	protected:
		LONG m_width;
		LONG m_height;
		bool m_showCursor;
		bool m_fullScreen;
		bool m_initialized;
		bool m_shouldResize;
		bool m_bound;
		float m_aspectRatio;
		std::string m_title;

	protected:
		Input m_input;
		WNDCLASSEX m_windowClassInfo;
		HWND m_windowHandler;
		RECT m_windowRectangle;
		IDXGISwapChain* m_swapChain;
	};
}


