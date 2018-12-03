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

			DankDevice(ID3D11Device* pdevice);
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

			virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext(UINT ContextFlags,
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

			virtual void STDMETHODCALLTYPE GetImmediateContext(_Outptr_  ID3D11DeviceContext** ppImmediateContext);

			virtual HRESULT STDMETHODCALLTYPE SetExceptionMode(UINT RaiseFlags);

			virtual UINT STDMETHODCALLTYPE GetExceptionMode(void);

		protected:
			ID3D11Device1* m_device;
		};

		class DankDeviceContext : public ID3D11DeviceContext
		{
		public:
			DankDeviceContext(ID3D11DeviceContext* pDeviceContext);
			virtual ~DankDeviceContext();

			virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void** ppvObject);

			virtual ULONG STDMETHODCALLTYPE AddRef(void);

			virtual ULONG STDMETHODCALLTYPE Release(void);

			virtual void STDMETHODCALLTYPE GetDevice(_Outptr_  ID3D11Device **ppDevice);

			virtual HRESULT STDMETHODCALLTYPE GetPrivateData(_In_  REFGUID guid,
				_Inout_  UINT *pDataSize,
				_Out_writes_bytes_opt_(*pDataSize)  void *pData);

			virtual HRESULT STDMETHODCALLTYPE SetPrivateData(_In_  REFGUID guid,
				_In_  UINT DataSize,
				_In_reads_bytes_opt_(DataSize)  const void *pData);

			virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(_In_  REFGUID guid,
				_In_opt_  const IUnknown *pData);

			virtual void STDMETHODCALLTYPE VSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE PSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE PSSetShader(_In_opt_  ID3D11PixelShader *pPixelShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE PSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE VSSetShader(
				_In_opt_  ID3D11VertexShader *pVertexShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE DrawIndexed(_In_  UINT IndexCount,
				_In_  UINT StartIndexLocation,
				_In_  INT BaseVertexLocation);

			virtual void STDMETHODCALLTYPE Draw(_In_  UINT VertexCount,
				_In_  UINT StartVertexLocation);

			virtual HRESULT STDMETHODCALLTYPE Map(_In_  ID3D11Resource *pResource,
				_In_  UINT Subresource,
				_In_  D3D11_MAP MapType,
				_In_  UINT MapFlags,
				_Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource);

			virtual void STDMETHODCALLTYPE Unmap(
				_In_  ID3D11Resource *pResource,
				_In_  UINT Subresource);

			virtual void STDMETHODCALLTYPE PSSetConstantBuffers(
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE IASetInputLayout(
				_In_opt_  ID3D11InputLayout *pInputLayout);

			virtual void STDMETHODCALLTYPE IASetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,
				_In_reads_opt_(NumBuffers)  const UINT *pStrides,
				_In_reads_opt_(NumBuffers)  const UINT *pOffsets);

			virtual void STDMETHODCALLTYPE IASetIndexBuffer(_In_opt_  ID3D11Buffer *pIndexBuffer,
				_In_  DXGI_FORMAT Format,
				_In_  UINT Offset);

			virtual void STDMETHODCALLTYPE DrawIndexedInstanced(_In_  UINT IndexCountPerInstance,
				_In_  UINT InstanceCount,
				_In_  UINT StartIndexLocation,
				_In_  INT BaseVertexLocation,
				_In_  UINT StartInstanceLocation);

			virtual void STDMETHODCALLTYPE DrawInstanced(_In_  UINT VertexCountPerInstance,
				_In_  UINT InstanceCount,
				_In_  UINT StartVertexLocation,
				_In_  UINT StartInstanceLocation);

			virtual void STDMETHODCALLTYPE GSSetConstantBuffers(
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE GSSetShader(_In_opt_  ID3D11GeometryShader *pShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE IASetPrimitiveTopology(_In_  D3D11_PRIMITIVE_TOPOLOGY Topology);

			virtual void STDMETHODCALLTYPE VSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE VSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE Begin(_In_  ID3D11Asynchronous *pAsync);

			virtual void STDMETHODCALLTYPE End(_In_  ID3D11Asynchronous *pAsync);

			virtual HRESULT STDMETHODCALLTYPE GetData(_In_  ID3D11Asynchronous *pAsync,
				_Out_writes_bytes_opt_(DataSize)  void *pData,
				_In_  UINT DataSize,
				_In_  UINT GetDataFlags);

			virtual void STDMETHODCALLTYPE SetPredication(_In_opt_  ID3D11Predicate *pPredicate,
				_In_  BOOL PredicateValue);

			virtual void STDMETHODCALLTYPE GSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE GSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE OMSetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
				_In_opt_  ID3D11DepthStencilView *pDepthStencilView);

			virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews(_In_  UINT NumRTVs,
				_In_reads_opt_(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
				_In_opt_  ID3D11DepthStencilView *pDepthStencilView,
				_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT UAVStartSlot,
				_In_  UINT NumUAVs,
				_In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
				_In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts);

			virtual void STDMETHODCALLTYPE OMSetBlendState(_In_opt_  ID3D11BlendState *pBlendState,
				_In_opt_  const FLOAT BlendFactor[4],
				_In_  UINT SampleMask);

			virtual void STDMETHODCALLTYPE OMSetDepthStencilState(_In_opt_  ID3D11DepthStencilState *pDepthStencilState,
				_In_  UINT StencilRef);

			virtual void STDMETHODCALLTYPE SOSetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
				_In_reads_opt_(NumBuffers)  const UINT *pOffsets);

			virtual void STDMETHODCALLTYPE DrawAuto(void);

			virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs,
				_In_  UINT AlignedByteOffsetForArgs);

			virtual void STDMETHODCALLTYPE DrawInstancedIndirect(_In_  ID3D11Buffer *pBufferForArgs,
				_In_  UINT AlignedByteOffsetForArgs);

			virtual void STDMETHODCALLTYPE Dispatch(_In_  UINT ThreadGroupCountX,
				_In_  UINT ThreadGroupCountY,
				_In_  UINT ThreadGroupCountZ);

			virtual void STDMETHODCALLTYPE DispatchIndirect(_In_  ID3D11Buffer *pBufferForArgs,
				_In_  UINT AlignedByteOffsetForArgs);

			virtual void STDMETHODCALLTYPE RSSetState(_In_opt_  ID3D11RasterizerState *pRasterizerState);

			virtual void STDMETHODCALLTYPE RSSetViewports(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
				_In_reads_opt_(NumViewports)  const D3D11_VIEWPORT *pViewports);

			virtual void STDMETHODCALLTYPE RSSetScissorRects(_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
				_In_reads_opt_(NumRects)  const D3D11_RECT *pRects);

			virtual void STDMETHODCALLTYPE CopySubresourceRegion(_In_  ID3D11Resource *pDstResource,
				_In_  UINT DstSubresource,
				_In_  UINT DstX,
				_In_  UINT DstY,
				_In_  UINT DstZ,
				_In_  ID3D11Resource *pSrcResource,
				_In_  UINT SrcSubresource,
				_In_opt_  const D3D11_BOX *pSrcBox);

			virtual void STDMETHODCALLTYPE CopyResource(_In_  ID3D11Resource *pDstResource,
				_In_  ID3D11Resource *pSrcResource);

			virtual void STDMETHODCALLTYPE UpdateSubresource(_In_  ID3D11Resource *pDstResource,
				_In_  UINT DstSubresource,
				_In_opt_  const D3D11_BOX *pDstBox,
				_In_  const void *pSrcData,
				_In_  UINT SrcRowPitch,
				_In_  UINT SrcDepthPitch);

			virtual void STDMETHODCALLTYPE CopyStructureCount(_In_  ID3D11Buffer *pDstBuffer,
				_In_  UINT DstAlignedByteOffset,
				_In_  ID3D11UnorderedAccessView *pSrcView);

			virtual void STDMETHODCALLTYPE ClearRenderTargetView(_In_  ID3D11RenderTargetView *pRenderTargetView,
				_In_  const FLOAT ColorRGBA[4]);

			virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView,
				_In_  const UINT Values[4]);

			virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat(_In_  ID3D11UnorderedAccessView *pUnorderedAccessView,
				_In_  const FLOAT Values[4]);

			virtual void STDMETHODCALLTYPE ClearDepthStencilView(_In_  ID3D11DepthStencilView *pDepthStencilView,
				_In_  UINT ClearFlags,
				_In_  FLOAT Depth,
				_In_  UINT8 Stencil);

			virtual void STDMETHODCALLTYPE GenerateMips(_In_  ID3D11ShaderResourceView *pShaderResourceView);

			virtual void STDMETHODCALLTYPE SetResourceMinLOD(_In_  ID3D11Resource *pResource,
				FLOAT MinLOD);

			virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD(_In_  ID3D11Resource *pResource);

			virtual void STDMETHODCALLTYPE ResolveSubresource(_In_  ID3D11Resource *pDstResource,
				_In_  UINT DstSubresource,
				_In_  ID3D11Resource *pSrcResource,
				_In_  UINT SrcSubresource,
				_In_  DXGI_FORMAT Format);

			virtual void STDMETHODCALLTYPE ExecuteCommandList(_In_  ID3D11CommandList *pCommandList,
				BOOL RestoreContextState);

			virtual void STDMETHODCALLTYPE HSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE HSSetShader(_In_opt_  ID3D11HullShader *pHullShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE HSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE HSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE DSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE DSSetShader(_In_opt_  ID3D11DomainShader *pDomainShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE DSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE DSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE CSSetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs,
				_In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
				_In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts);

			virtual void STDMETHODCALLTYPE CSSetShader(_In_opt_  ID3D11ComputeShader *pComputeShader,
				_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
				UINT NumClassInstances);

			virtual void STDMETHODCALLTYPE CSSetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

			virtual void STDMETHODCALLTYPE CSSetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

			virtual void STDMETHODCALLTYPE VSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE PSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE PSGetShader(_Outptr_result_maybenull_  ID3D11PixelShader **ppPixelShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE PSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE VSGetShader(_Outptr_result_maybenull_  ID3D11VertexShader **ppVertexShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE PSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE IAGetInputLayout(_Outptr_result_maybenull_  ID3D11InputLayout **ppInputLayout);

			virtual void STDMETHODCALLTYPE IAGetVertexBuffers(_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
				_Out_writes_opt_(NumBuffers)  UINT *pStrides,
				_Out_writes_opt_(NumBuffers)  UINT *pOffsets);

			virtual void STDMETHODCALLTYPE IAGetIndexBuffer(_Outptr_opt_result_maybenull_  ID3D11Buffer **pIndexBuffer,
				_Out_opt_  DXGI_FORMAT *Format,
				_Out_opt_  UINT *Offset);

			virtual void STDMETHODCALLTYPE GSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE GSGetShader(_Outptr_result_maybenull_  ID3D11GeometryShader **ppGeometryShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology(_Out_  D3D11_PRIMITIVE_TOPOLOGY *pTopology);

			virtual void STDMETHODCALLTYPE VSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE VSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE GetPredication(_Outptr_opt_result_maybenull_  ID3D11Predicate **ppPredicate,
				_Out_opt_  BOOL *pPredicateValue);

			virtual void STDMETHODCALLTYPE GSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE GSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE OMGetRenderTargets(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
				_Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView);

			virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews(_In_range_(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumRTVs,
				_Out_writes_opt_(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
				_Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView,
				_In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT UAVStartSlot,
				_In_range_(0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot)  UINT NumUAVs,
				_Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

			virtual void STDMETHODCALLTYPE OMGetBlendState(_Outptr_opt_result_maybenull_  ID3D11BlendState **ppBlendState,
				_Out_opt_  FLOAT BlendFactor[4],
				_Out_opt_  UINT *pSampleMask);

			virtual void STDMETHODCALLTYPE OMGetDepthStencilState(_Outptr_opt_result_maybenull_  ID3D11DepthStencilState **ppDepthStencilState,
				_Out_opt_  UINT *pStencilRef);

			virtual void STDMETHODCALLTYPE SOGetTargets(_In_range_(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppSOTargets);

			virtual void STDMETHODCALLTYPE RSGetState(_Outptr_result_maybenull_  ID3D11RasterizerState **ppRasterizerState);

			virtual void STDMETHODCALLTYPE RSGetViewports(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
				_Out_writes_opt_(*pNumViewports)  D3D11_VIEWPORT *pViewports);

			virtual void STDMETHODCALLTYPE RSGetScissorRects(_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
				_Out_writes_opt_(*pNumRects)  D3D11_RECT *pRects);

			virtual void STDMETHODCALLTYPE HSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE HSGetShader(_Outptr_result_maybenull_  ID3D11HullShader **ppHullShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE HSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE HSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE DSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE DSGetShader(_Outptr_result_maybenull_  ID3D11DomainShader **ppDomainShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE DSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE DSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE CSGetShaderResources(_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
				_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews);

			virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews(_In_range_(0, D3D11_1_UAV_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_1_UAV_SLOT_COUNT - StartSlot)  UINT NumUAVs,
				_Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

			virtual void STDMETHODCALLTYPE CSGetShader(_Outptr_result_maybenull_  ID3D11ComputeShader **ppComputeShader,
				_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
				_Inout_opt_  UINT *pNumClassInstances);

			virtual void STDMETHODCALLTYPE CSGetSamplers(_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)  UINT NumSamplers,
				_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers);

			virtual void STDMETHODCALLTYPE CSGetConstantBuffers(_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)  UINT StartSlot,
				_In_range_(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)  UINT NumBuffers,
				_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

			virtual void STDMETHODCALLTYPE ClearState(void);

			virtual void STDMETHODCALLTYPE Flush(void);

			virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType(void);

			virtual UINT STDMETHODCALLTYPE GetContextFlags(void);

			virtual HRESULT STDMETHODCALLTYPE FinishCommandList(BOOL RestoreDeferredContextState,
				_COM_Outptr_opt_  ID3D11CommandList **ppCommandList);

		private:
			ID3D11DeviceContext1* m_deviceContext;
		};
	}
}

inline void CreateDankDevice(_In_opt_ IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	ID3D11Device** ppDevice,
	UINT SDKVersion,
	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel)
{
	*ppDevice = new thomas::utils::DankDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pFeatureLevel);
}