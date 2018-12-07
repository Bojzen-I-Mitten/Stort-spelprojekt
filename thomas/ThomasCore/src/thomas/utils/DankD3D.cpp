#include "DankD3D.h"
#include "../Common.h"
#include "GpuProfiler.h"
#include "ProfileManager.h"

#include <string>
namespace thomas
{
	namespace utils
	{
		//Device
		DankDevice::DankDevice(_In_opt_ IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel)
		{
			ID3D11Device* device;
			D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, &device, pFeatureLevel, NULL);
			device->QueryInterface(IID_PPV_ARGS(&m_device));
			SAFE_RELEASE(device);
		}

		DankDevice::DankDevice(ID3D11Device* pdevice)
		{
			pdevice->QueryInterface(IID_PPV_ARGS(&m_device));
		}

		DankDevice::~DankDevice()
		{
			
		}

		HRESULT __stdcall DankDevice::QueryInterface(REFIID riid, void ** ppvObject)
		{
			return m_device->QueryInterface(riid, ppvObject);
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
			HRESULT hr = m_device->CreateBuffer(pDesc, pInitialData, ppBuffer);
			profiling::ProfileManager::storeVramSample("Buffer", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture1D(const D3D11_TEXTURE1D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture1D ** ppTexture1D)
		{
			HRESULT hr = m_device->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
			profiling::ProfileManager::storeVramSample("Texture1D", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture2D(const D3D11_TEXTURE2D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture2D ** ppTexture2D)
		{
			HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
			profiling::ProfileManager::storeVramSample("Texture2D", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture2D(std::string name, const D3D11_TEXTURE2D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture2D ** ppTexture2D)
		{
			HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateTexture3D(const D3D11_TEXTURE3D_DESC * pDesc, const D3D11_SUBRESOURCE_DATA * pInitialData, ID3D11Texture3D ** ppTexture3D)
		{
			HRESULT hr = m_device->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
			profiling::ProfileManager::storeVramSample("Texture3D", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateShaderResourceView(ID3D11Resource * pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc, ID3D11ShaderResourceView ** ppSRView)
		{
			HRESULT hr = m_device->CreateShaderResourceView(pResource, pDesc, ppSRView);
			profiling::ProfileManager::storeVramSample("SRV", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateShaderResourceView(std::string name, ID3D11Resource * pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC * pDesc, ID3D11ShaderResourceView ** ppSRView)
		{
			HRESULT hr = m_device->CreateShaderResourceView(pResource, pDesc, ppSRView);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateUnorderedAccessView(ID3D11Resource * pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC * pDesc, ID3D11UnorderedAccessView ** ppUAView)
		{
			HRESULT hr = m_device->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
			profiling::ProfileManager::storeVramSample("UAV", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRenderTargetView(ID3D11Resource * pResource, const D3D11_RENDER_TARGET_VIEW_DESC * pDesc, ID3D11RenderTargetView ** ppRTView)
		{
			HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);
			profiling::ProfileManager::storeVramSample("RTV", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRenderTargetView(std::string name, ID3D11Resource * pResource, const D3D11_RENDER_TARGET_VIEW_DESC * pDesc, ID3D11RenderTargetView ** ppRTView)
		{
			HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);
			profiling::ProfileManager::storeVramSample(name, profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDepthStencilView(ID3D11Resource * pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC * pDesc, ID3D11DepthStencilView ** ppDepthStencilView)
		{
			HRESULT hr = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
			profiling::ProfileManager::storeVramSample("DSV", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC * pInputElementDescs, UINT NumElements, const void * pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout ** ppInputLayout)
		{
			HRESULT hr = m_device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
			profiling::ProfileManager::storeVramSample("InputLayout", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateVertexShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11VertexShader ** ppVertexShader)
		{
			HRESULT hr = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
			profiling::ProfileManager::storeVramSample("VS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateGeometryShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			HRESULT hr = m_device->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
			profiling::ProfileManager::storeVramSample("GS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateGeometryShaderWithStreamOutput(const void * pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY * pSODeclaration, UINT NumEntries, const UINT * pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage * pClassLinkage, ID3D11GeometryShader ** ppGeometryShader)
		{
			HRESULT hr = m_device->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
			profiling::ProfileManager::storeVramSample("GS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreatePixelShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11PixelShader ** ppPixelShader)
		{
			HRESULT hr = m_device->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
			profiling::ProfileManager::storeVramSample("PS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateHullShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11HullShader ** ppHullShader)
		{
			HRESULT hr = m_device->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
			profiling::ProfileManager::storeVramSample("HS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDomainShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11DomainShader ** ppDomainShader)
		{
			HRESULT hr = m_device->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
			profiling::ProfileManager::storeVramSample("DS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateComputeShader(const void * pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage * pClassLinkage, ID3D11ComputeShader ** ppComputeShader)
		{
			HRESULT hr = m_device->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
			profiling::ProfileManager::storeVramSample("CS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateClassLinkage(ID3D11ClassLinkage ** ppLinkage)
		{
			HRESULT hr = m_device->CreateClassLinkage(ppLinkage);
			profiling::ProfileManager::storeVramSample("CL", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateBlendState(const D3D11_BLEND_DESC * pBlendStateDesc, ID3D11BlendState ** ppBlendState)
		{
			HRESULT hr = m_device->CreateBlendState(pBlendStateDesc, ppBlendState);
			profiling::ProfileManager::storeVramSample("BlendState", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC * pDepthStencilDesc, ID3D11DepthStencilState ** ppDepthStencilState)
		{
			HRESULT hr = m_device->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
			profiling::ProfileManager::storeVramSample("DSS", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateRasterizerState(const D3D11_RASTERIZER_DESC * pRasterizerDesc, ID3D11RasterizerState ** ppRasterizerState)
		{
			HRESULT hr = m_device->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
			profiling::ProfileManager::storeVramSample("RasterizerState", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateSamplerState(const D3D11_SAMPLER_DESC * pSamplerDesc, ID3D11SamplerState ** ppSamplerState)
		{
			HRESULT hr = m_device->CreateSamplerState(pSamplerDesc, ppSamplerState);
			profiling::ProfileManager::storeVramSample("SamplerState", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateQuery(const D3D11_QUERY_DESC * pQueryDesc, ID3D11Query ** ppQuery)
		{
			HRESULT hr = m_device->CreateQuery(pQueryDesc, ppQuery);
			profiling::ProfileManager::storeVramSample("Query", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreatePredicate(const D3D11_QUERY_DESC * pPredicateDesc, ID3D11Predicate ** ppPredicate)
		{
			HRESULT hr = m_device->CreatePredicate(pPredicateDesc, ppPredicate);
			profiling::ProfileManager::storeVramSample("Prediction", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateCounter(const D3D11_COUNTER_DESC * pCounterDesc, ID3D11Counter ** ppCounter)
		{
			HRESULT hr = m_device->CreateCounter(pCounterDesc, ppCounter);
			profiling::ProfileManager::storeVramSample("Counter", profiling::GpuProfiler::Instance()->GetMemoryUsage());
			return hr;
		}
		HRESULT __stdcall DankDevice::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext ** ppDeferredContext)
		{
			ID3D11DeviceContext* deviceContext;
			HRESULT hr = m_device->CreateDeferredContext(ContextFlags, &deviceContext);
			*ppDeferredContext = new DankDeviceContext(deviceContext);
			SAFE_RELEASE(deviceContext);
			return hr;
		}
		HRESULT __stdcall DankDevice::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void ** ppResource)
		{
			return m_device->OpenSharedResource(hResource, ReturnedInterface, ppResource);
		}
		HRESULT __stdcall DankDevice::CheckFormatSupport(DXGI_FORMAT Format, UINT * pFormatSupport)
		{
			return m_device->CheckFormatSupport(Format, pFormatSupport);
		}
		HRESULT __stdcall DankDevice::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT * pNumQualityLevels)
		{
			return m_device->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
		}
		void __stdcall DankDevice::CheckCounterInfo(D3D11_COUNTER_INFO * pCounterInfo)
		{
			m_device->CheckCounterInfo(pCounterInfo);
		}
		HRESULT __stdcall DankDevice::CheckCounter(const D3D11_COUNTER_DESC * pDesc, D3D11_COUNTER_TYPE * pType, UINT * pActiveCounters, LPSTR szName, UINT * pNameLength, LPSTR szUnits, UINT * pUnitsLength, LPSTR szDescription, UINT * pDescriptionLength)
		{
			return m_device->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
		}
		HRESULT __stdcall DankDevice::CheckFeatureSupport(D3D11_FEATURE Feature, void * pFeatureSupportData, UINT FeatureSupportDataSize)
		{
			return m_device->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
		}
		HRESULT __stdcall DankDevice::GetPrivateData(REFGUID guid, UINT * pDataSize, void * pData)
		{
			return m_device->GetPrivateData(guid, pDataSize, pData);
		}
		HRESULT __stdcall DankDevice::SetPrivateData(REFGUID guid, UINT DataSize, const void * pData)
		{
			return m_device->SetPrivateData(guid, DataSize, pData);
		}
		HRESULT __stdcall DankDevice::SetPrivateDataInterface(REFGUID guid, const IUnknown * pData)
		{
			return m_device->SetPrivateDataInterface(guid, pData);
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
			return m_device->GetDeviceRemovedReason();
		}
		void __stdcall DankDevice::GetImmediateContext(ID3D11DeviceContext ** ppImmediateContext)
		{
			ID3D11DeviceContext* deviceContext;
			m_device->GetImmediateContext(&deviceContext);
			*ppImmediateContext = new DankDeviceContext(deviceContext);
			SAFE_RELEASE(deviceContext);
		}
		HRESULT __stdcall DankDevice::SetExceptionMode(UINT RaiseFlags)
		{
			return m_device->SetExceptionMode(RaiseFlags);
		}
		UINT __stdcall DankDevice::GetExceptionMode(void)
		{
			return m_device->GetExceptionMode();
		}

		//Device Context
		DankDeviceContext::DankDeviceContext(ID3D11DeviceContext* pDeviceContext)
		{
			pDeviceContext->QueryInterface(IID_PPV_ARGS(&m_deviceContext));
		}
		DankDeviceContext::~DankDeviceContext()
		{
	
		}
		HRESULT __stdcall DankDeviceContext::QueryInterface(REFIID riid, void ** ppvObject)
		{
			return m_deviceContext->QueryInterface(riid, ppvObject);
		}
		ULONG __stdcall DankDeviceContext::AddRef(void)
		{
			return m_deviceContext->AddRef();
		}
		ULONG __stdcall DankDeviceContext::Release(void)
		{
			ULONG count = m_deviceContext->Release();
			if (count == 0)
			{
				delete(this);
			}

			return count;
		}
		void __stdcall DankDeviceContext::GetDevice(ID3D11Device ** ppDevice)
		{
			m_deviceContext->GetDevice(ppDevice);
			DankDevice* device = new DankDevice(*ppDevice);
			*ppDevice = device;
		}
		HRESULT __stdcall DankDeviceContext::GetPrivateData(REFGUID guid, UINT * pDataSize, void * pData)
		{
			return m_deviceContext->GetPrivateData(guid, pDataSize, pData);
		}
		HRESULT __stdcall DankDeviceContext::SetPrivateData(REFGUID guid, UINT DataSize, const void * pData)
		{
			return m_deviceContext->SetPrivateData(guid, DataSize, pData);
		}
		HRESULT __stdcall DankDeviceContext::SetPrivateDataInterface(REFGUID guid, const IUnknown * pData)
		{
			return m_deviceContext->SetPrivateDataInterface(guid, pData);
		}
		void __stdcall DankDeviceContext::VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}
		void __stdcall DankDeviceContext::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}
		void __stdcall DankDeviceContext::PSSetShader(ID3D11PixelShader * pPixelShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
		}
		void __stdcall DankDeviceContext::PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}
		void __stdcall DankDeviceContext::VSSetShader(ID3D11VertexShader * pVertexShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
		}
		void __stdcall DankDeviceContext::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
		{
			m_deviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		}
		void __stdcall DankDeviceContext::Draw(UINT VertexCount, UINT StartVertexLocation)
		{
			m_deviceContext->Draw(VertexCount, StartVertexLocation);
		}
		HRESULT __stdcall DankDeviceContext::Map(ID3D11Resource * pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE * pMappedResource)
		{
			return m_deviceContext->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
		}
		void __stdcall DankDeviceContext::Unmap(ID3D11Resource * pResource, UINT Subresource)
		{
			m_deviceContext->Unmap(pResource, Subresource);
		}
		void __stdcall DankDeviceContext::PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}
		void __stdcall DankDeviceContext::IASetInputLayout(ID3D11InputLayout * pInputLayout)
		{
			m_deviceContext->IASetInputLayout(pInputLayout);
		}
		void __stdcall DankDeviceContext::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppVertexBuffers, const UINT * pStrides, const UINT * pOffsets)
		{
			m_deviceContext->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
		}
		void __stdcall DankDeviceContext::IASetIndexBuffer(ID3D11Buffer * pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
		{
			m_deviceContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
		}
		void __stdcall DankDeviceContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
		{
			m_deviceContext->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartInstanceLocation, BaseVertexLocation, StartInstanceLocation);
		}
		void __stdcall DankDeviceContext::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
		{
			m_deviceContext->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
		}
		void __stdcall DankDeviceContext::GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}
		void __stdcall DankDeviceContext::GSSetShader(ID3D11GeometryShader * pShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->GSSetShader(pShader, ppClassInstances, NumClassInstances);
		}
		void __stdcall DankDeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
		{
			m_deviceContext->IASetPrimitiveTopology(Topology);
		}
		void __stdcall DankDeviceContext::VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}
		void __stdcall DankDeviceContext::VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}
		void __stdcall DankDeviceContext::Begin(ID3D11Asynchronous * pAsync)
		{
			m_deviceContext->Begin(pAsync);
		}
		void __stdcall DankDeviceContext::End(ID3D11Asynchronous * pAsync)
		{
			m_deviceContext->End(pAsync);
		}
		HRESULT __stdcall DankDeviceContext::GetData(ID3D11Asynchronous * pAsync, void * pData, UINT DataSize, UINT GetDataFlags)
		{
			return m_deviceContext->GetData(pAsync, pData, DataSize, GetDataFlags);
		}
		void __stdcall DankDeviceContext::SetPredication(ID3D11Predicate * pPredicate, BOOL PredicateValue)
		{
			m_deviceContext->SetPredication(pPredicate, PredicateValue);
		}
		void __stdcall DankDeviceContext::GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}
		void __stdcall DankDeviceContext::GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}
		void __stdcall DankDeviceContext::OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView * const * ppRenderTargetViews, ID3D11DepthStencilView * pDepthStencilView)
		{
			m_deviceContext->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
		}
		void __stdcall DankDeviceContext::OMSetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView * const * ppRenderTargetViews, ID3D11DepthStencilView * pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView * const * ppUnorderedAccessViews, const UINT * pUAVInitialCounts)
		{
			m_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
		}
		void __stdcall DankDeviceContext::OMSetBlendState(ID3D11BlendState * pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
		{
			m_deviceContext->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
		}
		void __stdcall DankDeviceContext::OMSetDepthStencilState(ID3D11DepthStencilState * pDepthStencilState, UINT StencilRef)
		{
			m_deviceContext->OMSetDepthStencilState(pDepthStencilState, StencilRef);
		}

		void __stdcall DankDeviceContext::SOSetTargets(UINT NumBuffers, ID3D11Buffer * const * ppSOTargets, const UINT * pOffsets)
		{
			m_deviceContext->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
		}

		void __stdcall DankDeviceContext::DrawAuto(void)
		{
			m_deviceContext->DrawAuto();
		}

		void __stdcall DankDeviceContext::DrawIndexedInstancedIndirect(ID3D11Buffer * pBufferForArgs, UINT AlignedByteOffsetForArgs)
		{
			m_deviceContext->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
		}

		void __stdcall DankDeviceContext::DrawInstancedIndirect(ID3D11Buffer * pBufferForArgs, UINT AlignedByteOffsetForArgs)
		{
			m_deviceContext->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
		}

		void __stdcall DankDeviceContext::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
		{
			m_deviceContext->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
		}

		void __stdcall DankDeviceContext::DispatchIndirect(ID3D11Buffer * pBufferForArgs, UINT AlignedByteOffsetForArgs)
		{
			m_deviceContext->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
		}

		void __stdcall DankDeviceContext::RSSetState(ID3D11RasterizerState * pRasterizerState)
		{
			m_deviceContext->RSSetState(pRasterizerState);
		}

		void __stdcall DankDeviceContext::RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT * pViewports)
		{
			m_deviceContext->RSSetViewports(NumViewports, pViewports);
		}

		void __stdcall DankDeviceContext::RSSetScissorRects(UINT NumRects, const D3D11_RECT * pRects)
		{
			m_deviceContext->RSSetScissorRects(NumRects, pRects);
		}

		void __stdcall DankDeviceContext::CopySubresourceRegion(ID3D11Resource * pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource * pSrcResource, UINT SrcSubresource, const D3D11_BOX * pSrcBox)
		{
			m_deviceContext->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
		}

		void __stdcall DankDeviceContext::CopyResource(ID3D11Resource * pDstResource, ID3D11Resource * pSrcResource)
		{
			m_deviceContext->CopyResource(pDstResource, pSrcResource);
		}

		void __stdcall DankDeviceContext::UpdateSubresource(ID3D11Resource * pDstResource, UINT DstSubresource, const D3D11_BOX * pDstBox, const void * pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
		{
			m_deviceContext->UpdateSubresource1(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch, 0);
		}

		void __stdcall DankDeviceContext::CopyStructureCount(ID3D11Buffer * pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView * pSrcView)
		{
			m_deviceContext->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
		}

		void __stdcall DankDeviceContext::ClearRenderTargetView(ID3D11RenderTargetView * pRenderTargetView, const FLOAT ColorRGBA[4])
		{
			m_deviceContext->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
		}

		void __stdcall DankDeviceContext::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView * pUnorderedAccessView, const UINT Values[4])
		{
			m_deviceContext->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
		}

		void __stdcall DankDeviceContext::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView * pUnorderedAccessView, const FLOAT Values[4])
		{
			m_deviceContext->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
		}

		void __stdcall DankDeviceContext::ClearDepthStencilView(ID3D11DepthStencilView * pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
		{
			m_deviceContext->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
		}

		void __stdcall DankDeviceContext::GenerateMips(ID3D11ShaderResourceView * pShaderResourceView)
		{
			m_deviceContext->GenerateMips(pShaderResourceView);
		}

		void __stdcall DankDeviceContext::SetResourceMinLOD(ID3D11Resource * pResource, FLOAT MinLOD)
		{
			m_deviceContext->SetResourceMinLOD(pResource, MinLOD);
		}

		FLOAT __stdcall DankDeviceContext::GetResourceMinLOD(ID3D11Resource * pResource)
		{
			return m_deviceContext->GetResourceMinLOD(pResource);
		}

		void __stdcall DankDeviceContext::ResolveSubresource(ID3D11Resource * pDstResource, UINT DstSubresource, ID3D11Resource * pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
		{
			m_deviceContext->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
		}

		void __stdcall DankDeviceContext::ExecuteCommandList(ID3D11CommandList * pCommandList, BOOL RestoreContextState)
		{
			m_deviceContext->ExecuteCommandList(pCommandList, RestoreContextState);
		}

		void __stdcall DankDeviceContext::HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::HSSetShader(ID3D11HullShader * pHullShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
		}

		void __stdcall DankDeviceContext::HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::DSSetShader(ID3D11DomainShader * pDomainShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
		}

		void __stdcall DankDeviceContext::DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews)
		{
			m_deviceContext->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView * const * ppUnorderedAccessViews, const UINT * pUAVInitialCounts)
		{
			m_deviceContext->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
		}

		void __stdcall DankDeviceContext::CSSetShader(ID3D11ComputeShader * pComputeShader, ID3D11ClassInstance * const * ppClassInstances, UINT NumClassInstances)
		{
			m_deviceContext->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
		}

		void __stdcall DankDeviceContext::CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState * const * ppSamplers)
		{
			m_deviceContext->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer * const * ppConstantBuffers)
		{
			m_deviceContext->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::PSGetShader(ID3D11PixelShader ** ppPixelShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::VSGetShader(ID3D11VertexShader ** ppVertexShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::IAGetInputLayout(ID3D11InputLayout ** ppInputLayout)
		{
			m_deviceContext->IAGetInputLayout(ppInputLayout);
		}

		void __stdcall DankDeviceContext::IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppVertexBuffers, UINT * pStrides, UINT * pOffsets)
		{
			m_deviceContext->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
		}

		void __stdcall DankDeviceContext::IAGetIndexBuffer(ID3D11Buffer ** pIndexBuffer, DXGI_FORMAT * Format, UINT * Offset)
		{
			m_deviceContext->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
		}

		void __stdcall DankDeviceContext::GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::GSGetShader(ID3D11GeometryShader ** ppGeometryShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY * pTopology)
		{
			m_deviceContext->IAGetPrimitiveTopology(pTopology);
		}

		void __stdcall DankDeviceContext::VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::GetPredication(ID3D11Predicate ** ppPredicate, BOOL * pPredicateValue)
		{
			m_deviceContext->GetPredication(ppPredicate, pPredicateValue);
		}

		void __stdcall DankDeviceContext::GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView ** ppRenderTargetViews, ID3D11DepthStencilView ** ppDepthStencilView)
		{
			m_deviceContext->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
		}

		void __stdcall DankDeviceContext::OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView ** ppRenderTargetViews, ID3D11DepthStencilView ** ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView ** ppUnorderedAccessViews)
		{
			m_deviceContext->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
		}

		void __stdcall DankDeviceContext::OMGetBlendState(ID3D11BlendState ** ppBlendState, FLOAT BlendFactor[4], UINT * pSampleMask)
		{
			m_deviceContext->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
		}

		void __stdcall DankDeviceContext::OMGetDepthStencilState(ID3D11DepthStencilState ** ppDepthStencilState, UINT * pStencilRef)
		{
			m_deviceContext->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
		}

		void __stdcall DankDeviceContext::SOGetTargets(UINT NumBuffers, ID3D11Buffer ** ppSOTargets)
		{
			m_deviceContext->SOGetTargets(NumBuffers, ppSOTargets);
		}

		void __stdcall DankDeviceContext::RSGetState(ID3D11RasterizerState ** ppRasterizerState)
		{
			m_deviceContext->RSGetState(ppRasterizerState);
		}

		void __stdcall DankDeviceContext::RSGetViewports(UINT * pNumViewports, D3D11_VIEWPORT * pViewports)
		{
			m_deviceContext->RSGetViewports(pNumViewports, pViewports);
		}

		void __stdcall DankDeviceContext::RSGetScissorRects(UINT * pNumRects, D3D11_RECT * pRects)
		{
			m_deviceContext->RSGetScissorRects(pNumRects, pRects);
		}

		void __stdcall DankDeviceContext::HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::HSGetShader(ID3D11HullShader ** ppHullShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::DSGetShader(ID3D11DomainShader ** ppDomainShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView ** ppShaderResourceViews)
		{
			m_deviceContext->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
		}

		void __stdcall DankDeviceContext::CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView ** ppUnorderedAccessViews)
		{
			m_deviceContext->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
		}

		void __stdcall DankDeviceContext::CSGetShader(ID3D11ComputeShader ** ppComputeShader, ID3D11ClassInstance ** ppClassInstances, UINT * pNumClassInstances)
		{
			m_deviceContext->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
		}

		void __stdcall DankDeviceContext::CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState ** ppSamplers)
		{
			m_deviceContext->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		}

		void __stdcall DankDeviceContext::CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer ** ppConstantBuffers)
		{
			m_deviceContext->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
		}

		void __stdcall DankDeviceContext::ClearState(void)
		{
			m_deviceContext->ClearState();
		}

		void __stdcall DankDeviceContext::Flush(void)
		{
			m_deviceContext->Flush();
		}

		D3D11_DEVICE_CONTEXT_TYPE __stdcall DankDeviceContext::GetType(void)
		{
			return m_deviceContext->GetType();
		}

		UINT __stdcall DankDeviceContext::GetContextFlags(void)
		{
			return m_deviceContext->GetContextFlags();
		}

		HRESULT __stdcall DankDeviceContext::FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList ** ppCommandList)
		{
			return m_deviceContext->FinishCommandList(RestoreDeferredContextState, ppCommandList);
		}

	}
}
