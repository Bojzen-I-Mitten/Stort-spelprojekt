#pragma once

#define FRAME_BUFFERS 3

#include "DankD3D.h"
#include <dxgidebug.h>
#include <string>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dCompiler.lib")

namespace thomas
{
	class Window;
	namespace utils
	{
		class D3D
		{
		public:
			static D3D* Instance();
			~D3D() = default;

			bool Init();
			void Destroy();
			
			bool CreateBackBuffer(IDXGISwapChain3* swapchain, ID3D11Texture2D*& backbuffer, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv,
				ID3D11DepthStencilView *& stencilView, ID3D11DepthStencilView*& depthStencilViewReadOnly);
			bool CreateDepthStencilView(ID3D11Texture2D* buffer, ID3D11DepthStencilView *& stencilView, ID3D11DepthStencilView*& depthStencilViewReadOnly, ID3D11ShaderResourceView *& depthBufferSRV);
			bool CreateDepthStencilTexture(int width, int height, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV);
			bool CreateDepthStencilTextureArray(int width, int height, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV, int arraySize);
			bool CreateSwapChain(LONG width, LONG height, HWND handle, IDXGISwapChain3*& swapchain);
			bool CreateRenderTarget(ID3D11Texture2D* backbuffer, ID3D11Texture2D*& buffer, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv);
			bool CreateTexture(void* initData, int width, int height, DXGI_FORMAT format, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV, bool mipMaps, int mipLevels);
			bool CreateRenderTexture(int width, int height, DXGI_FORMAT format, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV, ID3D11RenderTargetView*& RTV);
			bool CreateTextureArray(void** initData, int width, int height, int arraySize, DXGI_FORMAT format, ID3D11Texture2D *& texArray, ID3D11ShaderResourceView *& SRV, bool mipMaps, int mipLevels);
			bool CreateTextureCubeMap(void ** initData, uint32_t dim, DXGI_FORMAT format, ID3D11Texture2D *& texure2D, ID3D11ShaderResourceView *& SRV, bool mipMaps, int mipLevels);
			bool CreateQuery(D3D11_QUERY type, ID3D11Query*& query);

			bool D3D::CreateDepthStencilState(D3D11_COMPARISON_FUNC func, bool depth, ID3D11DepthStencilState*& depthStencilState);
			ID3D11RasterizerState* CreateRasterizer(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode);

		public:
			void FinishCommandList(ID3D11CommandList*& commandList);
			void ResetCommandList(ID3D11CommandList*& commandList);
			void ExecuteCommandList(ID3D11CommandList* commandList);

			void EnterCriticalSection();
			void ExitCriticalSection();

			void EnableMultithreadProtection(bool value);

		public:
			ID3D11Device* GetDevice();
			ID3D11DeviceContext* GetDeviceContextDeferred();
			ID3D11DeviceContext* GetDeviceContextImmediate();
			IDXGIDevice1* GetDxgiDevice();
			IDXGIAdapter3* GetDxgiAdapter();

		public:
			bool LoadTextureFromFile(std::string fileName, _Outptr_opt_ ID3D11Resource*& texture, _Outptr_opt_ ID3D11ShaderResourceView*& textureView);
			bool LoadCubeTextureFromFile(std::string fileName, _Outptr_opt_ ID3D11Resource*& texture, _Outptr_opt_ ID3D11ShaderResourceView*& textureView);
			void CreateTextureAndViews(UINT width, UINT height, DXGI_FORMAT format, ID3D11Texture2D*& tex, ID3D11ShaderResourceView*& SRV, ID3D11RenderTargetView*& RTV);
			void CreateBufferAndUAV(void* data, UINT byte_width, UINT byte_stride, ID3D11Buffer*& buffer, ID3D11UnorderedAccessView*& UAV, ID3D11ShaderResourceView*& SRV);
			void CreateCPUReadBufferAndUAV(void* data, UINT byte_width, UINT byte_stride, ID3D11Buffer*& buffer, ID3D11UnorderedAccessView*& UAV);
			ID3D11Buffer* CreateStagingBuffer(UINT byte_width, UINT byte_stride);
			
		private:
			D3D() = default;
			void CreateDebugInterface();
			bool CreateDxgiInterface();
			bool CreateDeviceContext();
			bool CreateMultiThreadedInterface();
			
		private:
			ID3D11Device* m_device;
			ID3D11DeviceContext* m_deviceContextImmediate;
			ID3D11DeviceContext* m_deviceContextDeferred;
			ID3D11Multithread* m_multiThreaded;

			IDXGIDebug* m_dxgiDebug;
			IDXGIDevice1* m_dxgiDevice;
			IDXGIAdapter3* m_dxgiAdapter;
		};
	}
}