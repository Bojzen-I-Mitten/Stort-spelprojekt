#include "DankD3D.h"
#include "../Common.h"
#include "GpuProfiler.h"
#include "ProfileManager.h"

#include <string>
namespace thomas
{
	namespace utils
	{
		DankDevice::DankDevice(_In_opt_ IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel)
		{
			D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, &m_device, pFeatureLevel, NULL);
		}

		DankDevice::~DankDevice()
		{
			
		}

		HRESULT __stdcall DankDevice::QueryInterface(REFIID riid, void ** ppvObject)
		{
			HRESULT hr = m_device->QueryInterface(riid, ppvObject);
			return hr;
		}

		ULONG __stdcall DankDevice::AddRef(void)
		{
			return m_device->AddRef();
		}

		ULONG __stdcall DankDevice::Release(void)
		{
			ULONG count = m_device->Release();
			if (count == 0)
			{
				delete(this);
			}
	
			return count;
		}

		HRESULT __stdcall DankDevice::CreateBuffer(const D3D11_BUFFER_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Buffer ** ppBuffer)
		{
			static int m_count;
			HRESULT hr = m_device->CreateBuffer(pDesc, pInitialData, ppBuffer);

			m_count+=1;
			std::string name = "Buffer" + std::to_string(m_count);
			
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();
			
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture1D(const D3D11_TEXTURE1D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture1D ** ppTexture1D)
		{
			static int m_count;
			HRESULT hr = m_device->CreateTexture1D(pDesc, pInitialData, ppTexture1D);

			m_count += 1;
			std::string name = "Texture1D" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture2D(const D3D11_TEXTURE2D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture2D ** ppTexture2D)
		{
			static int m_count;
			HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

			m_count += 1;
			std::string name = "Texture2D" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture2D(std::string name, const D3D11_TEXTURE2D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture2D ** ppTexture2D)
		{
			static int m_count;
			HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

			m_count += 1;
			name = name + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture3D(const D3D11_TEXTURE3D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture3D ** ppTexture3D)
		{
			static int m_count;
			HRESULT hr = m_device->CreateTexture3D(pDesc, pInitialData, ppTexture3D);

			m_count += 1;
			std::string name = "Texture3D" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateShaderResourceView(ID3D11Resource * pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc, ID3D11ShaderResourceView ** ppSRView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateShaderResourceView(pResource, pDesc, ppSRView);
			
			m_count += 1;
			std::string name = "SRV" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateShaderResourceView(std::string name, ID3D11Resource * pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc, ID3D11ShaderResourceView ** ppSRView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateShaderResourceView(pResource, pDesc, ppSRView);

			m_count += 1;
			name = name + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateUnorderedAccessView(ID3D11Resource * pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC * pDesc, ID3D11UnorderedAccessView ** ppUAView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateUnorderedAccessView(pResource, pDesc, ppUAView);

			m_count += 1;
			std::string name = "UAV" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();
			
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRenderTargetView(ID3D11Resource * pResource, const D3D11_RENDER_TARGET_VIEW_DESC * pDesc, ID3D11RenderTargetView ** ppRTView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

			m_count += 1;
			std::string name = "RTV" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRenderTargetView(std::string name, ID3D11Resource * pResource, const D3D11_RENDER_TARGET_VIEW_DESC * pDesc, ID3D11RenderTargetView ** ppRTView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

			m_count += 1;
			name = name + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDepthStencilView(ID3D11Resource * pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC * pDesc, ID3D11DepthStencilView ** ppDepthStencilView)
		{
			static int m_count;
			HRESULT hr = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

			m_count += 1;
			std::string name = "DSV" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout ** ppInputLayout)
		{
			static int m_count;
			HRESULT hr = m_device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);

			m_count += 1;
			std::string name = "InputLayout" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateVertexShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11VertexShader ** ppVertexShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);

			m_count += 1;
			std::string name = "VS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateGeometryShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);

			m_count += 1;
			std::string name = "GS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateGeometryShaderWithStreamOutput(const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, const UINT * pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);

			m_count += 1;
			std::string name = "GS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreatePixelShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11PixelShader ** ppPixelShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);

			m_count += 1;
			std::string name = "PS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateHullShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11HullShader ** ppHullShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);

			m_count += 1;
			std::string name = "HS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDomainShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11DomainShader ** ppDomainShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);

			m_count += 1;
			std::string name = "DS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateComputeShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11ComputeShader ** ppComputeShader)
		{
			static int m_count;
			HRESULT hr = m_device->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);

			m_count += 1;
			std::string name = "CS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateClassLinkage(ID3D11ClassLinkage ** ppLinkage)
		{
			static int m_count;
			HRESULT hr = m_device->CreateClassLinkage(ppLinkage);

			m_count += 1;
			std::string name = "CL" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateBlendState(const D3D11_BLEND_DESC * pBlendStateDesc, ID3D11BlendState ** ppBlendState)
		{
			static int m_count;
			HRESULT hr = m_device->CreateBlendState(pBlendStateDesc, ppBlendState);

			m_count += 1;
			std::string name = "BlendState" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC * pDepthStencilDesc, ID3D11DepthStencilState ** ppDepthStencilState)
		{
			static int m_count;
			HRESULT hr = m_device->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);

			m_count += 1;
			std::string name = "DSS" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRasterizerState(const D3D11_RASTERIZER_DESC * pRasterizerDesc, ID3D11RasterizerState ** ppRasterizerState)
		{
			static int m_count;
			HRESULT hr = m_device->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);

			m_count += 1;
			std::string name = "RasterizerState" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateSamplerState(const D3D11_SAMPLER_DESC * pSamplerDesc, ID3D11SamplerState ** ppSamplerState)
		{
			static int m_count;
			HRESULT hr = m_device->CreateSamplerState(pSamplerDesc, ppSamplerState);

			m_count += 1;
			std::string name = "SamplerState" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateQuery(const D3D11_QUERY_DESC * pQueryDesc, ID3D11Query ** ppQuery)
		{
			static int m_count;
			HRESULT hr = m_device->CreateQuery(pQueryDesc, ppQuery);
			m_count += 1;
			std::string name = "Query" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();
			return hr;
		}
		HRESULT __stdcall DankDevice::CreatePredicate(const D3D11_QUERY_DESC * pPredicateDesc, ID3D11Predicate ** ppPredicate)
		{
			static int m_count;
			HRESULT hr = m_device->CreatePredicate(pPredicateDesc, ppPredicate);

			m_count += 1;
			std::string name = "Prediction" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateCounter(const D3D11_COUNTER_DESC * pCounterDesc, ID3D11Counter ** ppCounter)
		{
			static int m_count;
			HRESULT hr = m_device->CreateCounter(pCounterDesc, ppCounter);

			m_count += 1;
			std::string name = "Prediction" + std::to_string(m_count);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext ** ppDeferredContext)
		{
			HRESULT hr = m_device->CreateDeferredContext(ContextFlags, ppDeferredContext);
			return hr;
		}
		HRESULT __stdcall DankDevice::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void ** ppResource)
		{
			HRESULT hr = m_device->OpenSharedResource(hResource, ReturnedInterface, ppResource);
			return hr;
		}
		HRESULT __stdcall DankDevice::CheckFormatSupport(DXGI_FORMAT Format, UINT * pFormatSupport)
		{
			HRESULT hr = m_device->CheckFormatSupport(Format, pFormatSupport);
			return hr;
		}
		HRESULT __stdcall DankDevice::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels)
		{
			HRESULT hr = m_device->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
			return hr;
		}
		void __stdcall DankDevice::CheckCounterInfo(D3D11_COUNTER_INFO * pCounterInfo)
		{
			m_device->CheckCounterInfo(pCounterInfo);
		}
		HRESULT __stdcall DankDevice::CheckCounter(const D3D11_COUNTER_DESC * pDesc, D3D11_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength)
		{
			HRESULT hr = m_device->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
			return hr;
		}
		HRESULT __stdcall DankDevice::CheckFeatureSupport(D3D11_FEATURE Feature, void * pFeatureSupportData, UINT FeatureSupportDataSize)
		{
			HRESULT hr = m_device->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
			return hr;
		}
		HRESULT __stdcall DankDevice::GetPrivateData(REFGUID guid, UINT * pDataSize, void * pData)
		{
			HRESULT hr = m_device->GetPrivateData(guid, pDataSize, pData);
			return hr;
		}
		HRESULT __stdcall DankDevice::SetPrivateData(REFGUID guid, UINT DataSize, const void * pData)
		{
			HRESULT hr = m_device->SetPrivateData(guid, DataSize, pData);
			return hr;
		}
		HRESULT __stdcall DankDevice::SetPrivateDataInterface(REFGUID guid, const IUnknown * pData)
		{
			HRESULT hr = m_device->SetPrivateDataInterface(guid, pData);
			return hr;
		}
		D3D_FEATURE_LEVEL __stdcall DankDevice::GetFeatureLevel(void)
		{
			return m_device->GetFeatureLevel();
		}
		UINT __stdcall DankDevice::GetCreationFlags(void)
		{
			return m_device->GetCreationFlags();
		}
		HRESULT __stdcall DankDevice::GetDeviceRemovedReason(void)
		{
			HRESULT hr = m_device->GetDeviceRemovedReason();
			return hr;
		}
		void __stdcall DankDevice::GetImmediateContext(ID3D11DeviceContext ** ppImmediateContext)
		{
			m_device->GetImmediateContext(ppImmediateContext);
			m_deviceContext = *ppImmediateContext;
		}
		HRESULT __stdcall DankDevice::SetExceptionMode(UINT RaiseFlags)
		{
			HRESULT hr = m_device->SetExceptionMode(RaiseFlags);
			return hr;
		}
		UINT __stdcall DankDevice::GetExceptionMode(void)
		{
			return m_device->GetExceptionMode();
		}
	}
}
