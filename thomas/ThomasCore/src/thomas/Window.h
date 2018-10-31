#pragma once
#include <Windows.h>
#include <vector>
#include <d3d11_4.h>
#include "Input.h"

namespace thomas 
{
	class Window
	{
	protected:
		struct DXBuffers
		{
			//Buffer pointers to render targets 3D rendering and GUI
			ID3D11Texture2D* buffer = nullptr;

			ID3D11RenderTargetView* RTV = nullptr;
			ID3D11ShaderResourceView* SRV = nullptr;
			ID3D11DepthStencilState* depthStencilState = nullptr;
			ID3D11DepthStencilView* depthStencilView = nullptr;
			ID3D11DepthStencilView* depthStencilViewReadOnly = nullptr;


			//Buffer pointer to swapChain back buffer
			ID3D11Texture2D* backbuffer = nullptr;
			
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
		/*void BindGUITarget();*/
		void Clear();
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
		HANDLE m_frameLatancyWaitableObject;
		IDXGISwapChain3* m_swapChain;
	};
}


