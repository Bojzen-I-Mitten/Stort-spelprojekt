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
			ID3D11CommandList* commandList = nullptr;
			ID3D11Texture2D* buffer[2];

			ID3D11RenderTargetView* RTV[2];
			ID3D11ShaderResourceView* SRV[2];
			ID3D11DepthStencilView* depthStencilView[2];
			ID3D11DepthStencilView* depthStencilViewReadOnly[2];
			ID3D11DepthStencilState* depthStencilState;
			ID3D11ShaderResourceView* depthBufferSRV;

		} m_dx;

	public:
		Window(HWND& hwnd, HWND parent, LONG width, LONG height, std::string name);
		virtual ~Window();

	public:
		virtual void UpdateWindow();
		void QueueResize();
		void QueueWindowStyleChange();
		void BindBackBuffer();
		void BindRenderTarget();
		void ResolveRenderTarget();
		void WaitOnSwapChain();
		virtual void BeginFrame();
		virtual void EndFrame();

	public:
		bool ShouldResize();
		bool ShouldWindowStyleChange();
		bool Initialized();
		bool ChangeWindowShowState(int nCmdShow);
		bool IntersectBounds(int x, int y) const;

	public:
		bool SetHeight(const LONG & height);
		bool SetWidth(const LONG & width);
		bool SetSize(const LONG & height, const LONG & width);
		void SetCursor(const bool & visible);
		void SetFullscreen(bool fullscreen);
		void SetBorderless(bool borderless);
		
	public:
		static LRESULT CALLBACK EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		bool GetFullScreen();
		bool GetBorderless();
		Input* GetInput();
		LONG GetHeight() const;
		LONG GetWidth() const;
		RECT GetBounds() const;
		LONG GetHorizontalResolution() const;
		LONG GetVerticalResolution() const;
		HWND GetWindowHandler() const;
		float GetRealAspectRatio() const;

		virtual bool IsEditor() { return false; }
	
	protected:
		bool InitDxBuffers();
		bool Resize();
		bool ResizeBackBuffer();
		bool ChangeWindowStyle();
		void Clear();
		void Create(HWND& hwnd, HWND parent);
		bool CreateSwapChain();
		virtual void Present();

	protected:
		LONG m_width;
		LONG m_height;
		DWORD m_windowStyle;
		bool m_showCursor;
		bool m_fullScreen;
		bool m_borderless;
		bool m_initialized;
		bool m_shouldStyleChange;
		bool m_shouldResize;
		
		float m_aspectRatio;
		std::string m_title;

	protected:
		Input m_input;
		WNDCLASSEX m_windowClassInfo;
		HINSTANCE m_hInstance;
		HWND m_windowHandler;
		RECT m_windowRectangle;

		IDXGIOutput* m_output;
		IDXGISwapChain3* m_swapChain;
		HANDLE m_waitableObject;
	};
}


