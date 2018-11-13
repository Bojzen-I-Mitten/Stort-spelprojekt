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
		struct DX
		{
			ID3D11CommandList* commandList[FRAME_BUFFERS];
			ID3D11Texture2D* buffer[2][FRAME_BUFFERS];

			ID3D11RenderTargetView* RTV[2][FRAME_BUFFERS];
			ID3D11ShaderResourceView* SRV[2][FRAME_BUFFERS];
			ID3D11DepthStencilView* depthStencilView[2][FRAME_BUFFERS];
			ID3D11DepthStencilView* depthStencilViewReadOnly[2][FRAME_BUFFERS];
			ID3D11DepthStencilState* depthStencilState;
			ID3D11ShaderResourceView* depthBufferSRV[FRAME_BUFFERS];

		} m_dx;

	public:
		Window(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title);
		Window(HWND hWnd);
		virtual ~Window();

	public:
		virtual void UpdateWindow();
		void QueueResize();
		void BindBackBuffer();
		void BindRenderTarget();
		void ResolveRenderTarget();
		void WaitOnSwapChain();
		virtual void BeginFrame();
		virtual void EndFrame();

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
		void Clear();
		virtual void Present();

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
		int m_frameIndex;
	};
}


