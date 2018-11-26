#include "D3DDevice.h"
#include "../Common.h"
#include "GpuProfiler.h"
#include "ProfileManager.h"

namespace thomas
{
	namespace utils
	{
		D3DDevice::D3DDevice(_In_opt_ IDXGIAdapter* pAdapter,
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

		D3DDevice::~D3DDevice()
		{
			
		}

		HRESULT __stdcall D3DDevice::QueryInterface(REFIID riid, void ** ppvObject)
		{
			HRESULT hr = m_device->QueryInterface(riid, ppvObject);
			return hr;
		}

		ULONG __stdcall D3DDevice::AddRef(void)
		{
			return m_device->AddRef();
		}

		ULONG __stdcall D3DDevice::Release(void)
		{
			ULONG count = m_device->Release();
			if (count == 0)
			{
				delete(this);
			}
	
			return count;
		}

		HRESULT __stdcall D3DDevice::CreateBuffer(const D3D11_BUFFER_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Buffer ** ppBuffer)
		{
			HRESULT hr = m_device->CreateBuffer(pDesc, pInitialData, ppBuffer);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();
			
			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateTexture1D(const D3D11_TEXTURE1D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture1D ** ppTexture1D)
		{
			HRESULT hr = m_device->CreateTexture1D(pDesc, pInitialData, ppTexture1D);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateTexture2D(const D3D11_TEXTURE2D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture2D ** ppTexture2D)
		{
			HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
			
			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateTexture3D(const D3D11_TEXTURE3D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture3D ** ppTexture3D)
		{
			HRESULT hr = m_device->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
			
			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateShaderResourceView(ID3D11Resource * pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc, ID3D11ShaderResourceView ** ppSRView)
		{
			HRESULT hr = m_device->CreateShaderResourceView(pResource, pDesc, ppSRView);
			
			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateUnorderedAccessView(ID3D11Resource * pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC * pDesc, ID3D11UnorderedAccessView ** ppUAView)
		{
			HRESULT hr = m_device->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
			
			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();
			
			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateRenderTargetView(ID3D11Resource * pResource, const D3D11_RENDER_TARGET_VIEW_DESC * pDesc, ID3D11RenderTargetView ** ppRTView)
		{
			HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateDepthStencilView(ID3D11Resource * pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC * pDesc, ID3D11DepthStencilView ** ppDepthStencilView)
		{
			HRESULT hr = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout ** ppInputLayout)
		{
			HRESULT hr = m_device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateVertexShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11VertexShader ** ppVertexShader)
		{
			HRESULT hr = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateGeometryShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			HRESULT hr = m_device->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateGeometryShaderWithStreamOutput(const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, const UINT * pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			HRESULT hr = m_device->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreatePixelShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11PixelShader ** ppPixelShader)
		{
			HRESULT hr = m_device->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateHullShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11HullShader ** ppHullShader)
		{
			HRESULT hr = m_device->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateDomainShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11DomainShader ** ppDomainShader)
		{
			HRESULT hr = m_device->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateComputeShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11ComputeShader ** ppComputeShader)
		{
			HRESULT hr = m_device->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateClassLinkage(ID3D11ClassLinkage ** ppLinkage)
		{
			HRESULT hr = m_device->CreateClassLinkage(ppLinkage);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateBlendState(const D3D11_BLEND_DESC * pBlendStateDesc, ID3D11BlendState ** ppBlendState)
		{
			HRESULT hr = m_device->CreateBlendState(pBlendStateDesc, ppBlendState);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC * pDepthStencilDesc, ID3D11DepthStencilState ** ppDepthStencilState)
		{
			HRESULT hr = m_device->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateRasterizerState(const D3D11_RASTERIZER_DESC * pRasterizerDesc, ID3D11RasterizerState ** ppRasterizerState)
		{
			HRESULT hr = m_device->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateSamplerState(const D3D11_SAMPLER_DESC * pSamplerDesc, ID3D11SamplerState ** ppSamplerState)
		{
			HRESULT hr = m_device->CreateSamplerState(pSamplerDesc, ppSamplerState);

			profiling::ProfileManager::storeVramSample(profiling::GpuProfiler::Instance()->GetMemoryUsage());
			m_deviceContext->Flush();

			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateQuery(const D3D11_QUERY_DESC * pQueryDesc, ID3D11Query ** ppQuery)
		{
			HRESULT hr = m_device->CreateQuery(pQueryDesc, ppQuery);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CreatePredicate(const D3D11_QUERY_DESC * pPredicateDesc, ID3D11Predicate ** ppPredicate)
		{
			HRESULT hr = m_device->CreatePredicate(pPredicateDesc, ppPredicate);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateCounter(const D3D11_COUNTER_DESC * pCounterDesc, ID3D11Counter ** ppCounter)
		{
			HRESULT hr = m_device->CreateCounter(pCounterDesc, ppCounter);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext ** ppDeferredContext)
		{
			HRESULT hr = m_device->CreateDeferredContext(ContextFlags, ppDeferredContext);
			return hr;
		}
		HRESULT __stdcall D3DDevice::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void ** ppResource)
		{
			HRESULT hr = m_device->OpenSharedResource(hResource, ReturnedInterface, ppResource);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CheckFormatSupport(DXGI_FORMAT Format, UINT * pFormatSupport)
		{
			HRESULT hr = m_device->CheckFormatSupport(Format, pFormatSupport);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels)
		{
			HRESULT hr = m_device->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
			return hr;
		}
		void __stdcall D3DDevice::CheckCounterInfo(D3D11_COUNTER_INFO * pCounterInfo)
		{
			m_device->CheckCounterInfo(pCounterInfo);
		}
		HRESULT __stdcall D3DDevice::CheckCounter(const D3D11_COUNTER_DESC * pDesc, D3D11_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength)
		{
			HRESULT hr = m_device->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
			return hr;
		}
		HRESULT __stdcall D3DDevice::CheckFeatureSupport(D3D11_FEATURE Feature, void * pFeatureSupportData, UINT FeatureSupportDataSize)
		{
			HRESULT hr = m_device->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
			return hr;
		}
		HRESULT __stdcall D3DDevice::GetPrivateData(REFGUID guid, UINT * pDataSize, void * pData)
		{
			HRESULT hr = m_device->GetPrivateData(guid, pDataSize, pData);
			return hr;
		}
		HRESULT __stdcall D3DDevice::SetPrivateData(REFGUID guid, UINT DataSize, const void * pData)
		{
			HRESULT hr = m_device->SetPrivateData(guid, DataSize, pData);
			return hr;
		}
		HRESULT __stdcall D3DDevice::SetPrivateDataInterface(REFGUID guid, const IUnknown * pData)
		{
			HRESULT hr = m_device->SetPrivateDataInterface(guid, pData);
			return hr;
		}
		D3D_FEATURE_LEVEL __stdcall D3DDevice::GetFeatureLevel(void)
		{
			return m_device->GetFeatureLevel();
		}
		UINT __stdcall D3DDevice::GetCreationFlags(void)
		{
			return m_device->GetCreationFlags();
		}
		HRESULT __stdcall D3DDevice::GetDeviceRemovedReason(void)
		{
			HRESULT hr = m_device->GetDeviceRemovedReason();
			return hr;
		}
		void __stdcall D3DDevice::GetImmediateContext(ID3D11DeviceContext ** ppImmediateContext)
		{
			m_device->GetImmediateContext(ppImmediateContext);
			m_deviceContext = *ppImmediateContext;
		}
		HRESULT __stdcall D3DDevice::SetExceptionMode(UINT RaiseFlags)
		{
			HRESULT hr = m_device->SetExceptionMode(RaiseFlags);
			return hr;
		}
		UINT __stdcall D3DDevice::GetExceptionMode(void)
		{
			return m_device->GetExceptionMode();
		}
	}
}
