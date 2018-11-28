#pragma once
#include <d3d11_4.h>
#include <dxgi1_4.h>
#include <string>

namespace thomas
{
	namespace utils
	{
		/*class DankSwapChain : public IDXGISwapChain
		{
		public:
			DankSwapChain();
			virtual ~DankSwapChain();

			virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags);

			virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer,
				_In_  REFIID riid,
				_COM_Outptr_  void **ppSurface);

			virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen,
				_In_opt_  IDXGIOutput *pTarget);

			virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(_Out_opt_  BOOL *pFullscreen,
				_COM_Outptr_opt_result_maybenull_  IDXGIOutput **ppTarget);

			virtual HRESULT STDMETHODCALLTYPE GetDesc(_Out_  DXGI_SWAP_CHAIN_DESC *pDesc);

			virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, 
				UINT Width,
				UINT Height,
				DXGI_FORMAT NewFormat,
				UINT SwapChainFlags);

			virtual HRESULT STDMETHODCALLTYPE ResizeTarget(_In_  const DXGI_MODE_DESC *pNewTargetParameters);

			virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(_COM_Outptr_  IDXGIOutput **ppOutput);

			virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(_Out_  DXGI_FRAME_STATISTICS *pStats);

			virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(_Out_  UINT *pLastPresentCount);

		private:
			IDXGISwapChain3* m_swapChain;
		};*/

		class DankDevice : public ID3D11Device
		{
		public:
			DankDevice(_In_opt_ IDXGIAdapter* pAdapter,
				D3D_DRIVER_TYPE DriverType,
				HMODULE Software,
				UINT Flags,
				_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
				UINT FeatureLevels,
				UINT SDKVersion,
				_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel);

			virtual ~DankDevice();

			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject);

			virtual ULONG STDMETHODCALLTYPE AddRef(void);

			virtual ULONG STDMETHODCALLTYPE Release(void);

			virtual HRESULT STDMETHODCALLTYPE CreateBuffer(_In_  const D3D11_BUFFER_DESC *pDesc,
				_In_opt_  const D3D11_SUBRESOURCE_DATA *pInitialData,
				_COM_Outptr_opt_  ID3D11Buffer **ppBuffer);

			virtual HRESULT STDMETHODCALLTYPE CreateTexture1D(_In_  const D3D11_TEXTURE1D_DESC *pDesc,
				_In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
				_COM_Outptr_opt_  ID3D11Texture1D **ppTexture1D);

			virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(_In_  const D3D11_TEXTURE2D_DESC *pDesc,
				_In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
				_COM_Outptr_opt_  ID3D11Texture2D **ppTexture2D);

			virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(_In_ std::string name, _In_  const D3D11_TEXTURE2D_DESC *pDesc,
				_In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
				_COM_Outptr_opt_  ID3D11Texture2D **ppTexture2D);

			virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(_In_  const D3D11_TEXTURE3D_DESC *pDesc,
				_In_reads_opt_(_Inexpressible_(pDesc->MipLevels))  const D3D11_SUBRESOURCE_DATA *pInitialData,
				_COM_Outptr_opt_  ID3D11Texture3D **ppTexture3D);

			virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(_In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11ShaderResourceView **ppSRView);

			virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(_In_ std::string name, _In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11ShaderResourceView **ppSRView);

			virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(_In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11UnorderedAccessView **ppUAView);

			virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(_In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11RenderTargetView **ppRTView);

			virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(_In_ std::string name, _In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11RenderTargetView **ppRTView);

			virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(_In_  ID3D11Resource *pResource,
				_In_opt_  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
				_COM_Outptr_opt_  ID3D11DepthStencilView **ppDepthStencilView);

			virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(_In_reads_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
				_In_range_(0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT)  UINT NumElements,
				_In_reads_(BytecodeLength)  const void *pShaderBytecodeWithInputSignature,
				_In_  SIZE_T BytecodeLength,
				_COM_Outptr_opt_  ID3D11InputLayout **ppInputLayout);

			virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11VertexShader **ppVertexShader);

			virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11GeometryShader **ppGeometryShader);

			virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_reads_opt_(NumEntries)  const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
				_In_range_(0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT)  UINT NumEntries,
				_In_reads_opt_(NumStrides)  const UINT *pBufferStrides,
				_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumStrides,
				_In_  UINT RasterizedStream,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11GeometryShader **ppGeometryShader);

			virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11PixelShader **ppPixelShader);

			virtual HRESULT STDMETHODCALLTYPE CreateHullShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11HullShader **ppHullShader);

			virtual HRESULT STDMETHODCALLTYPE CreateDomainShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11DomainShader **ppDomainShader);

			virtual HRESULT STDMETHODCALLTYPE CreateComputeShader(_In_reads_(BytecodeLength)  const void *pShaderBytecode,
				_In_  SIZE_T BytecodeLength,
				_In_opt_  ID3D11ClassLinkage *pClassLinkage,
				_COM_Outptr_opt_  ID3D11ComputeShader **ppComputeShader);

			virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage(_COM_Outptr_  ID3D11ClassLinkage **ppLinkage);

			virtual HRESULT STDMETHODCALLTYPE CreateBlendState(_In_  const D3D11_BLEND_DESC *pBlendStateDesc,
				_COM_Outptr_opt_  ID3D11BlendState **ppBlendState);

			virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(_In_  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
				_COM_Outptr_opt_  ID3D11DepthStencilState **ppDepthStencilState);

			virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(_In_  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
				_COM_Outptr_opt_  ID3D11RasterizerState **ppRasterizerState);

			virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(_In_  const D3D11_SAMPLER_DESC *pSamplerDesc,
				_COM_Outptr_opt_  ID3D11SamplerState **ppSamplerState);

			virtual HRESULT STDMETHODCALLTYPE CreateQuery(_In_  const D3D11_QUERY_DESC *pQueryDesc,
				_COM_Outptr_opt_  ID3D11Query **ppQuery);

			virtual HRESULT STDMETHODCALLTYPE CreatePredicate(_In_  const D3D11_QUERY_DESC *pPredicateDesc,
				_COM_Outptr_opt_  ID3D11Predicate **ppPredicate);

			virtual HRESULT STDMETHODCALLTYPE CreateCounter(_In_  const D3D11_COUNTER_DESC *pCounterDesc,
				_COM_Outptr_opt_  ID3D11Counter **ppCounter);

			virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext(
				UINT ContextFlags,
				_COM_Outptr_opt_  ID3D11DeviceContext **ppDeferredContext);

			virtual HRESULT STDMETHODCALLTYPE OpenSharedResource(_In_  HANDLE hResource,
				_In_  REFIID ReturnedInterface,
				_COM_Outptr_opt_  void **ppResource);

			virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(_In_  DXGI_FORMAT Format,
				_Out_  UINT *pFormatSupport);

			virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(_In_  DXGI_FORMAT Format,
				_In_  UINT SampleCount,
				_Out_  UINT *pNumQualityLevels);

			virtual void STDMETHODCALLTYPE CheckCounterInfo(_Out_  D3D11_COUNTER_INFO *pCounterInfo);

			virtual HRESULT STDMETHODCALLTYPE CheckCounter(_In_  const D3D11_COUNTER_DESC *pDesc,
				_Out_  D3D11_COUNTER_TYPE *pType,
				_Out_  UINT *pActiveCounters,
				_Out_writes_opt_(*pNameLength)  LPSTR szName,
				_Inout_opt_  UINT *pNameLength,
				_Out_writes_opt_(*pUnitsLength)  LPSTR szUnits,
				_Inout_opt_  UINT *pUnitsLength,
				_Out_writes_opt_(*pDescriptionLength)  LPSTR szDescription,
				_Inout_opt_  UINT *pDescriptionLength);

			virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport(D3D11_FEATURE Feature,
				_Out_writes_bytes_(FeatureSupportDataSize)  void *pFeatureSupportData,
				UINT FeatureSupportDataSize);

			virtual HRESULT STDMETHODCALLTYPE GetPrivateData(_In_  REFGUID guid,
				_Inout_  UINT *pDataSize,
				_Out_writes_bytes_opt_(*pDataSize)  void *pData);

			virtual HRESULT STDMETHODCALLTYPE SetPrivateData(_In_  REFGUID guid,
				_In_  UINT DataSize,
				_In_reads_bytes_opt_(DataSize)  const void *pData);

			virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_  REFGUID guid,
				_In_opt_  const IUnknown *pData);

			virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel(void);

			virtual UINT STDMETHODCALLTYPE GetCreationFlags(void);

			virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason(void);

			virtual void STDMETHODCALLTYPE GetImmediateContext(_Outptr_  ID3D11DeviceContext **ppImmediateContext);

			virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags);

			virtual UINT STDMETHODCALLTYPE GetExceptionMode(void);

		private:
			ID3D11Device* m_device;
			ID3D11DeviceContext* m_deviceContext;
		};
	}
}

inline void CreateDankDevice(_In_opt_ IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	thomas::utils::DankDevice** ppDevice,
	UINT SDKVersion,
	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel)
{
	*ppDevice = new thomas::utils::DankDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pFeatureLevel);
}