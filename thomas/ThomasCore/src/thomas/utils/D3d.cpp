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
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = mipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			if (mipMaps)
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			else
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			

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

			if (SUCCEEDED(hr))
			{
				hr = m_device->CreateShaderResourceView(tex, &viewDesc, &SRV);
				if (SUCCEEDED(hr))
					return true;
			}

			LOG_HR(hr);
			return false;
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
				for (int i = 0; i < arraySize; i++) 
				{

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

			if (SUCCEEDED(hr))
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
				ZeroMemory(&viewDesc, sizeof(viewDesc));
				viewDesc.Format = textureDesc.Format;
				viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				viewDesc.Texture2DArray.ArraySize = arraySize;
				viewDesc.Texture2DArray.FirstArraySlice = 0;
				viewDesc.Texture2DArray.MipLevels = mipLevels;
				viewDesc.Texture2DArray.MostDetailedMip = 0;

				hr = m_device->CreateShaderResourceView(texure2D, &viewDesc, &SRV);
				if (SUCCEEDED(hr))
					return true;
			}
			
			LOG_HR(hr);
			return false;
		}

		bool D3D::CreateSwapChain(LONG width, LONG height, HWND handle, IDXGISwapChain3*& swapchain)
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

						scd.BufferCount = FRAME_BUFFERS;
						scd.BufferDesc.Height = height;
						scd.BufferDesc.Width = width;
						scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
						scd.OutputWindow = handle;
						scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
						scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

						scd.SampleDesc.Count = 1; //Make this costomizable!!!
						scd.SampleDesc.Quality = 0;
						scd.Windowed = TRUE;
						scd.BufferDesc.RefreshRate.Numerator = 0; // change 0 to numerator for vsync
						scd.BufferDesc.RefreshRate.Denominator = 1; // change 1 to denominator for vynsc

						IDXGISwapChain* dxgiSwapChain;
						hr = dxgiFactory->CreateSwapChain(m_device, &scd, &dxgiSwapChain);
						if (SUCCEEDED(hr))
						{
							dxgiSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapchain);
							swapchain->SetMaximumFrameLatency(FRAME_BUFFERS - 1);
							SAFE_RELEASE(dxgiSwapChain);
						}

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

			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;
			HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &device, &fl, &deviceContext);
			if (SUCCEEDED(hr))
			{
				hr = device->QueryInterface(__uuidof(ID3D11Device3), (void**)&m_device);
				if (SUCCEEDED(hr))
				{
					hr = deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext3), (void**)&m_deviceContext);
					if (SUCCEEDED(hr))
					{
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
				}
			}

			SAFE_RELEASE(device);
			SAFE_RELEASE(deviceContext);
			LOG_HR(hr);
			return false;
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

		bool D3D::CreateBackBuffer(IDXGISwapChain3* swapchain, ID3D11RenderTargetView*& RTV, ID3D11ShaderResourceView*& SRV, ID3D11Texture2D*& backBuffer, ID3D11RenderTargetView*& backBufferRTV, ID3D11Texture2D*& buffer)
		{
			HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
			if (SUCCEEDED(hr))
			{
				m_device->CreateRenderTargetView(backBuffer, NULL, &backBufferRTV);

				D3D11_TEXTURE2D_DESC backBufferDesc;
				backBuffer->GetDesc(&backBufferDesc);
				backBufferDesc.SampleDesc.Count = THOMAS_AA_COUNT;
				backBufferDesc.SampleDesc.Quality = THOMAS_AA_QUALITY;
				backBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				backBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

				hr = m_device->CreateTexture2D(&backBufferDesc, NULL, &buffer);
				if (SUCCEEDED(hr))
				{
					D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
					ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
					RTVDesc.Format = backBufferDesc.Format;
					RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

					hr = m_device->CreateRenderTargetView(buffer, &RTVDesc, &RTV);
					if (FAILED(hr))
					{
						LOG_HR(hr);
						return false;
					}

					D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
					ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
					SRVDesc.Format = backBufferDesc.Format;
					SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

					hr = m_device->CreateShaderResourceView(buffer, &SRVDesc, &SRV);
					if (FAILED(hr))
					{
						LOG_HR(hr);
						return false;
					}
				}
			}

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

		bool D3D::CreateDepthStencilView(LONG width, LONG height, ID3D11DepthStencilView *& stencilView, ID3D11ShaderResourceView *& depthBufferSRV)
		{
			D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
			ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
			depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilBufferDesc.Width = width;
			depthStencilBufferDesc.Height = height;
			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.MipLevels = 1;
			depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilBufferDesc.SampleDesc.Count = THOMAS_AA_COUNT;
			depthStencilBufferDesc.SampleDesc.Quality = THOMAS_AA_QUALITY;

			ID3D11Texture2D* depthStencil;
			HRESULT hr = m_device->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencil);
			if (SUCCEEDED(hr))
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
				ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
				depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

				hr = m_device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &stencilView);
				if (FAILED(hr))
				{
					LOG_HR(hr);
					return false;
				}
			}

			SAFE_RELEASE(depthStencil);

			return true;
		}

		bool D3D::CreateDepthStencilView(LONG width, LONG height, ID3D11DepthStencilView *& stencilView, ID3D11DepthStencilView*& depthStencilViewReadOnly,
			ID3D11ShaderResourceView *& depthBufferSRV)
		{
			D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
			ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
			depthStencilBufferDesc.Width = width;
			depthStencilBufferDesc.Height = height;
			depthStencilBufferDesc.MipLevels = 1;
			depthStencilBufferDesc.ArraySize = 1;
			depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			depthStencilBufferDesc.SampleDesc.Count = 1;
			depthStencilBufferDesc.SampleDesc.Quality = 0;
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
			ZeroMemory(&depthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
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
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

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
		/*
		bool D3D::LoadTextureArrayFromFiles(std::vector<std::string>& fileNames, ID3D11Texture2D *& texure2D, ID3D11ShaderResourceView *& textureView)
		{
			unsigned nrOfTextures = fileNames.size();
			ID3D11Resource* tempTex;

			DirectX::ScratchImage data;
			byte** initData = new byte*[nrOfTextures];

			for (unsigned i = 0; i < nrOfTextures; ++i)
			{
				char* filename_c = new char[fileNames[i].length() + 1];
				strcpy_s(filename_c, fileNames[i].length() + 1, fileNames[i].c_str());
				char * extension_char = PathFindExtensionA(filename_c);
				std::string extension_string(extension_char);

				delete[] filename_c;
				HRESULT hr;
				if (extension_string == ".dds")
				{
					hr = DirectX::CreateDDSTextureFromFile(m_device, m_deviceContext, CA2W(fileNames[i].c_str()), &tempTex, nullptr);
				}
				else
				{
					hr = DirectX::CreateWICTextureFromFile(m_device, m_deviceContext, CA2W(fileNames[i].c_str()), &tempTex, nullptr);
				}

				if (FAILED(hr))
				{
					LOG("Failed to load texture " << i << ": " << fileNames[i] << " error: ");
					LOG_HR(hr);

					return false;
				}

				

				DirectX::ScratchImage firstData;
				hr = DirectX::CaptureTexture(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), tempTex, firstData);
				if (FAILED(hr))
				{
					LOG("Failed to capture texture " << i << ": " << fileNames[i] << "error: ");
					LOG_HR(hr);
					return false;
				}
				hr = DirectX::Resize(*firstData.GetImage(0,0,0), 256, 256, 0, firstData);
				if (FAILED(hr))
				{
					LOG("Failed to resize texture " << i << ": " << fileNames[i] << "error: ");
					LOG_HR(hr);
					return false;
				}
				hr = DirectX::Convert(*firstData.GetImage(0, 0, 0), DXGI_FORMAT_B8G8R8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, data);
				if (FAILED(hr))
				{
					LOG("Failed to convert texture " << i << ": " << fileNames[i] << "error: ");
					LOG_HR(hr);
					return false;
				}
				firstData.Release();

				initData[i] = data.GetPixels();
				tempTex->Release(); //??
			}

			CreateTextureArray((void**)initData, 256, 256, nrOfTextures, DXGI_FORMAT_B8G8R8A8_UNORM, texure2D, textureView, false, -1);

			return true;
		}*/

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