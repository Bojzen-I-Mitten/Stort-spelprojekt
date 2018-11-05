#pragma once
#include "D3D.h"
#include "../Common.h"
#include "../Window.h"
#include "..\ThomasCore.h"
#include "GpuProfiler.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/DDSTextureLoader.h"
#include <dxgi.h>
#include <d3d11_4.h>
#include <AtlBase.h>
#include <atlconv.h>
#include <comdef.h>
#include <string.h>  

#include <DirectXTex.h>

namespace thomas
{
	namespace utils
	{
		D3D D3D::s_D3D;

		bool D3D::CreateRenderTargetView(LONG width, LONG height, ID3D11RenderTargetView *& rtv, ID3D11ShaderResourceView *& srv)
		{
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_GENERATE_MIPS;

			D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = textureDesc.Format;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipLevels = 1;
			viewDesc.Texture2D.MostDetailedMip = 0;

			ID3D11Texture2D* quadTexture;
			m_device->CreateTexture2D(&textureDesc, NULL, &quadTexture);
			m_device->CreateShaderResourceView(quadTexture, &viewDesc, &srv);
			m_device->CreateRenderTargetView(quadTexture, NULL, &rtv);

			SAFE_RELEASE(quadTexture);

			return true;
		}

		bool D3D::CreateTexture(void* initData, int width, int height, DXGI_FORMAT format, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV, bool mipMaps, int mipLevels = 1)
		{
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = mipLevels;
			textureDesc.ArraySize = 1;
			textureDesc.Format = format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			if (mipMaps)
			{
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			}
			else
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = mipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = textureDesc.Format;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipLevels = mipLevels;
			viewDesc.Texture2D.MostDetailedMip = 0;

			HRESULT hr;
			if (initData)
			{
				D3D11_SUBRESOURCE_DATA TexInitData;
				ZeroMemory(&TexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
				TexInitData.pSysMem = initData;
				TexInitData.SysMemPitch = static_cast<UINT>(4 * width);
				TexInitData.SysMemSlicePitch = static_cast<UINT>(4 * width * height);
				hr = m_device->CreateTexture2D(&textureDesc, &TexInitData, &tex);
			}
			else
				hr = m_device->CreateTexture2D(&textureDesc, NULL, &tex);

			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			hr = m_device->CreateShaderResourceView(tex, &viewDesc, &SRV);
			if (FAILED(hr))
				return false;

			return true;
		}

		bool D3D::CreateTextureArray(void** initData, int width, int height, int arraySize, DXGI_FORMAT format, ID3D11Texture2D *& texure2D, ID3D11ShaderResourceView *& SRV, bool mipMaps, int mipLevels)
		{
			D3D11_TEXTURE2D_DESC textureDesc;

			ZeroMemory(&textureDesc, sizeof(textureDesc));
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = mipLevels;
			textureDesc.ArraySize = arraySize;
			textureDesc.Format = format;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = mipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			HRESULT hr;
			
			if (initData)
			{
				D3D11_SUBRESOURCE_DATA* texInitData = new D3D11_SUBRESOURCE_DATA[arraySize];
				for (int i = 0; i < arraySize; ++i) {

					D3D11_SUBRESOURCE_DATA sd = {};
					sd.pSysMem = initData[i];
					sd.SysMemPitch = static_cast<UINT>(4 * width);
					sd.SysMemSlicePitch = static_cast<UINT>(4 * width * height);

					texInitData[i] = sd;
				}
				hr = m_device->CreateTexture2D(&textureDesc, texInitData, &texure2D);
				delete[] texInitData;
			}
			else
				hr = m_device->CreateTexture2D(&textureDesc, NULL, &texure2D);

			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}
			D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
			ZeroMemory(&viewDesc, sizeof(viewDesc));
			viewDesc.Format = textureDesc.Format;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			viewDesc.Texture2DArray.ArraySize = arraySize;
			viewDesc.Texture2DArray.FirstArraySlice = 0;
			viewDesc.Texture2DArray.MipLevels = mipLevels;
			viewDesc.Texture2DArray.MostDetailedMip = 0;

			hr = m_device->CreateShaderResourceView(texure2D, &viewDesc, &SRV);
			if (FAILED(hr))
				return false;

			return true;
		}

		bool D3D::CreateSwapChain(LONG width, LONG height, HWND handle, IDXGISwapChain*& swapchain)
		{
			IDXGIDevice* dxgiDevice = nullptr;
			HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void **)& dxgiDevice);
			if (SUCCEEDED(hr))
			{
				IDXGIAdapter* dxgiAdapter = nullptr;
				hr = dxgiDevice->GetAdapter(&dxgiAdapter);
				if (SUCCEEDED(hr))
				{
					IDXGIFactory* dxgiFactory = nullptr;
					hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)& dxgiFactory);
					if (SUCCEEDED(hr))
					{
						DXGI_SWAP_CHAIN_DESC scd;
						ZeroMemory(&scd, sizeof(scd));

						scd.BufferCount = 2;
						scd.BufferDesc.Height = height;
						scd.BufferDesc.Width = width;
						scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
						scd.OutputWindow = handle;
						scd.Flags = 0;
						scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

						scd.SampleDesc.Count = THOMAS_AA_COUNT; //Make this costomizable!!!
						scd.SampleDesc.Quality = THOMAS_AA_QUALITY;
						scd.Windowed = TRUE;
						scd.BufferDesc.RefreshRate.Numerator = 0; // change 0 to numerator for vsync
						scd.BufferDesc.RefreshRate.Denominator = 1; // change 1 to denominator for vynsc

						hr = dxgiFactory->CreateSwapChain(m_device, &scd, &swapchain);

						SAFE_RELEASE(dxgiFactory);
					}
					SAFE_RELEASE(dxgiAdapter)
				}
				SAFE_RELEASE(dxgiDevice);
			}

			if (FAILED(hr))
				return false;

			return true;
		}

		D3D* D3D::Instance()
		{
			return &s_D3D;
		}

		bool D3D::Init()
		{
			CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

			const D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
			UINT createDeviceFlags = 0;
			D3D_FEATURE_LEVEL fl;

#ifdef _DEBUG_DX
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &m_device, &fl, &m_deviceContext);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			ID3D11Multithread* multi = nullptr;
			hr = m_device->QueryInterface(IID_PPV_ARGS(&multi));
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			multi->SetMultithreadProtected(TRUE);
			SAFE_RELEASE(multi);

#ifdef  _DEBUG_DX
			CreateDebugInterface();
#endif
			m_profiler = new profiling::GpuProfiler();
			return m_profiler->Init();
		}

		void D3D::CreateDebugInterface()
		{
			HRESULT hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
			if (FAILED(hr))
				LOG_HR(hr);
		}

		bool D3D::CreateQuery(D3D11_QUERY type, ID3D11Query *& query)
		{
			D3D11_QUERY_DESC desc;
			desc.Query = type;
			desc.MiscFlags = 0;

			HRESULT hr = m_device->CreateQuery(&desc, &query);
			return SUCCEEDED(hr);
		}

		void D3D::Destroy()
		{
			m_profiler->Destroy();
			delete m_profiler;

			m_deviceContext->ClearState();
			m_deviceContext->Flush();

			SAFE_RELEASE(m_deviceContext);
			SAFE_RELEASE(m_device);

			if (m_debug)
				m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

			SAFE_RELEASE(m_debug);
		}

		bool D3D::CreateBackBuffer(IDXGISwapChain* swapchain, ID3D11RenderTargetView*& backBuffer, ID3D11ShaderResourceView*& backbufferSRV)
		{
			ID3D11Texture2D* pbackBuffer;
			HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pbackBuffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			D3D11_TEXTURE2D_DESC backBufferDesc;
			pbackBuffer->GetDesc(&backBufferDesc);
			hr = m_device->CreateRenderTargetView(pbackBuffer, NULL, &backBuffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			hr = m_device->CreateShaderResourceView(pbackBuffer, NULL, &backbufferSRV);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			SAFE_RELEASE(pbackBuffer);
			return true;
		}

		ID3D11DepthStencilState* D3D::CreateDepthStencilState(D3D11_COMPARISON_FUNC func, bool depth)
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

			depthStencilDesc.DepthEnable = depth;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = func;
			depthStencilDesc.StencilReadMask = true;
			depthStencilDesc.StencilWriteMask = 0xFF;
			depthStencilDesc.StencilReadMask = 0xFF;

			// if front face
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// if back face
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			ID3D11DepthStencilState* stencilState = nullptr;
			HRESULT hr = m_device->CreateDepthStencilState(&depthStencilDesc, &stencilState);
			if (FAILED(hr))
				LOG_HR(hr);

			return stencilState;
		}

		bool D3D::CreateDepthStencilView(LONG width, LONG height, ID3D11DepthStencilView *& stencilView, ID3D11DepthStencilView*& depthStencilViewReadOnly,
			ID3D11ShaderResourceView *& depthBufferSRV)
		{
			D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
			ZeroMemory(&depthStencilBufferDesc, sizeof(depthStencilBufferDesc));
			depthStencilBufferDesc.Width = width;
			depthStencilBufferDesc.Height = height;
			depthStencilBufferDesc.MipLevels = 1;
			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			depthStencilBufferDesc.SampleDesc.Count = THOMAS_AA_COUNT;
			depthStencilBufferDesc.SampleDesc.Quality = THOMAS_AA_QUALITY;
			depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			depthStencilBufferDesc.CPUAccessFlags = 0;
			depthStencilBufferDesc.MiscFlags = 0;


			ID3D11Texture2D* depthStencilBuffer;
			HRESULT hr = m_device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilBuffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
			ZeroMemory(&depthViewDesc, sizeof(depthViewDesc));
			depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
#if THOMAS_AA_COUNT > 1
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#else
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
			depthViewDesc.Texture2D.MipSlice = 0;
			depthViewDesc.Flags = 0;

			hr = m_device->CreateDepthStencilView(depthStencilBuffer, &depthViewDesc, &stencilView);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			depthViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			hr = m_device->CreateDepthStencilView(depthStencilBuffer, &depthViewDesc, &depthStencilViewReadOnly);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
#if THOMAS_AA_COUNT > 1
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
#else
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
#endif
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;

			hr = m_device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &depthBufferSRV);
			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			SAFE_RELEASE(depthStencilBuffer);
			return true;

		}

		bool D3D::LoadTextureFromFile(std::string fileName,
			ID3D11Resource*& texture, ID3D11ShaderResourceView*& textureView)
		{
			// Convert from string to char in order to split by token
			// convert back from char, since its easier to compare strings than chars
			// Fuck you, thats why.
			char* filename_c = new char[fileName.length() + 1];
			strcpy_s(filename_c, fileName.length() + 1, fileName.c_str());
			char * extension_char = PathFindExtensionA(filename_c);
			std::string extension_string(extension_char);

			delete[] filename_c;

			HRESULT hr;
			if (extension_string == ".dds")
			{
				hr = DirectX::CreateDDSTextureFromFile(m_device, m_deviceContext, CA2W(fileName.c_str()), &texture, &textureView);
			}
			else
			{
				hr = DirectX::CreateWICTextureFromFile(m_device, m_deviceContext, CA2W(fileName.c_str()), &texture, &textureView);
			}

			if (FAILED(hr))
			{
				LOG("Failed to load texture: " << fileName << " error: ");
				LOG_HR(hr);
				return false;
			}

			return true;
		}
		

		bool D3D::LoadCubeTextureFromFile(std::string fileName, ID3D11Resource *& texture, ID3D11ShaderResourceView *& textureView)
		{
			char* filename_c = new char[fileName.length() + 1];
			strcpy_s(filename_c, fileName.length() + 1, fileName.c_str());

			char * extension_char = PathFindExtensionA(filename_c);
			std::string extension_string(extension_char);

			delete[] filename_c;

			HRESULT hr;
			if (extension_string == ".dds")
				hr = DirectX::CreateDDSTextureFromFileEx(m_device, CA2W(fileName.c_str()), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
					D3D11_RESOURCE_MISC_TEXTURECUBE, false, &texture, &textureView, nullptr);
			else
				hr = DirectX::CreateWICTextureFromFileEx(m_device, CA2W(fileName.c_str()), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
					D3D11_RESOURCE_MISC_TEXTURECUBE, 0, &texture, &textureView);

			if (FAILED(hr))
			{
				LOG_HR(hr);
				return false;
			}

			return true;
		}

		ID3D11RasterizerState* D3D::CreateRasterizer(D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode)
		{
			D3D11_RASTERIZER_DESC rasterDesc;

			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.CullMode = cullMode;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = false;
			rasterDesc.FillMode = fillMode;
			rasterDesc.FrontCounterClockwise = true;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;

			ID3D11RasterizerState* rasterState = nullptr;
			m_device->CreateRasterizerState(&rasterDesc, &rasterState);

			return rasterState;
		}

		ID3D11Device * D3D::GetDevice()
		{
			return m_device;
		}

		ID3D11DeviceContext * D3D::GetDeviceContext()
		{
			return m_deviceContext;
		}

		profiling::GpuProfiler* D3D::GetProfiler()
		{
			return m_profiler;
		}

		void D3D::CreateTextureAndViews(UINT width, UINT height, DXGI_FORMAT format, ID3D11Texture2D *& tex, ID3D11ShaderResourceView *& SRV, ID3D11RenderTargetView *& RTV)
		{
			// Create 2D texture
			D3D11_TEXTURE2D_DESC tex_desc;
			tex_desc.Width = width;
			tex_desc.Height = height;
			tex_desc.MipLevels = 0;
			tex_desc.ArraySize = 1;
			tex_desc.Format = format;
			tex_desc.SampleDesc.Count = 1;
			tex_desc.SampleDesc.Quality = 0;
			tex_desc.Usage = D3D11_USAGE_DEFAULT;
			tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			tex_desc.CPUAccessFlags = 0;
			tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			m_device->CreateTexture2D(&tex_desc, NULL, &tex);

			// Create shader resource view
			tex->GetDesc(&tex_desc);
			if (tex)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
				srv_desc.Format = format;
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;
				srv_desc.Texture2D.MostDetailedMip = 0;

				m_device->CreateShaderResourceView(tex, &srv_desc, &SRV);
			}

			// Create render target view
			if (SRV)
			{
				D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
				rtv_desc.Format = format;
				rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtv_desc.Texture2D.MipSlice = 0;

				m_device->CreateRenderTargetView(tex, &rtv_desc, &RTV);
			}
		}

		void D3D::CreateBufferAndUAV(void * data, UINT byte_width, UINT byte_stride, ID3D11Buffer *& buffer, ID3D11UnorderedAccessView *& UAV, ID3D11ShaderResourceView *& SRV)
		{
			D3D11_BUFFER_DESC buf_desc;
			buf_desc.ByteWidth = byte_width;
			buf_desc.Usage = D3D11_USAGE_DEFAULT;
			buf_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			buf_desc.CPUAccessFlags = 0;
			buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buf_desc.StructureByteStride = byte_stride;

			D3D11_SUBRESOURCE_DATA init_data = { data, 0, 0 };

			HRESULT hr = m_device->CreateBuffer(&buf_desc, data != NULL ? &init_data : NULL, &buffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
			}

			// Create undordered access view
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.FirstElement = 0;
			uav_desc.Buffer.NumElements = byte_width / byte_stride;
			uav_desc.Buffer.Flags = 0;

			hr = m_device->CreateUnorderedAccessView(buffer, &uav_desc, &UAV);
			if (FAILED(hr))
				LOG_HR(hr);

			// Create shader resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			srv_desc.Format = DXGI_FORMAT_UNKNOWN;
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srv_desc.Buffer.FirstElement = 0;
			srv_desc.Buffer.NumElements = byte_width / byte_stride;

			hr = m_device->CreateShaderResourceView(buffer, &srv_desc, &SRV);
			if (FAILED(hr))
			{
				LOG_HR(hr);
			}
		}

		void D3D::CreateCPUReadBufferAndUAV(void * data, UINT byte_width, UINT byte_stride, ID3D11Buffer *& buffer, ID3D11UnorderedAccessView *& UAV)
		{
			D3D11_BUFFER_DESC buf_desc;
			buf_desc.ByteWidth = byte_width;
			buf_desc.Usage = D3D11_USAGE_DEFAULT;
			buf_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			buf_desc.CPUAccessFlags = 0;
			buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buf_desc.StructureByteStride = byte_stride;

			D3D11_SUBRESOURCE_DATA init_data = { data, 0, 0 };

			HRESULT hr = m_device->CreateBuffer(&buf_desc, data != NULL ? &init_data : NULL, &buffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
			}

			// Create undordered access view
			D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav_desc.Buffer.FirstElement = 0;
			uav_desc.Buffer.NumElements = byte_width / byte_stride;
			uav_desc.Buffer.Flags = 0;

			hr = m_device->CreateUnorderedAccessView(buffer, &uav_desc, &UAV);
			if (FAILED(hr))
			{
				LOG_HR(hr);
			}
		}

		ID3D11Buffer * D3D::CreateStagingBuffer(UINT byte_width, UINT byte_stride)
		{
			D3D11_BUFFER_DESC buf_desc;
			buf_desc.ByteWidth = byte_width;
			buf_desc.Usage = D3D11_USAGE_STAGING;
			buf_desc.BindFlags = 0;
			buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buf_desc.StructureByteStride = byte_stride;

			ID3D11Buffer* buffer = nullptr;
			HRESULT hr = m_device->CreateBuffer(&buf_desc, NULL, &buffer);
			if (FAILED(hr))
			{
				LOG_HR(hr);
			}
			return buffer;
		}
	}
}