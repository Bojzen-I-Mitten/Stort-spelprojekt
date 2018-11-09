#pragma once
#include <Windows.h>
#include <vector>
#include "utils/D3D.h"
#include "Input.h"

namespace thomas 
{
	class Window
	{
	protected:
		struct DXBuffers
		{
			ID3D11Texture2D* buffer[2];

			ID3D11RenderTargetView* RTV[2] = { nullptr, nullptr };
			ID3D11ShaderResourceView* SRV[2] = { nullptr, nullptr };
			ID3D11DepthStencilState* depthStencilState = nullptr;
			ID3D11DepthStencilView* depthStencilView[2] = { nullptr, nullptr };
			ID3D11DepthStencilView* depthStencilViewReadOnly[2] = { nullptr, nullptr };
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
		void BindBackBuffer();
		void BindRenderTarget();
		void Clear();
		void ResolveRenderTarget();
		void WaitOnSwapChain();

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
		IDXGISwapChain* GetSwapChain() const;
		HWND GetWindowHandler() const;
		float GetRealAspectRatio() const;

		virtual bool IsEditor() { return false; }
	
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
		float m_aspectRatio;
		std::string m_title;

	protected:
		Input m_input;
		WNDCLASSEX m_windowClassInfo;
		HWND m_windowHandler;
		RECT m_windowRectangle;

		IDXGISwapChain3* m_swapChain;
	};
}


