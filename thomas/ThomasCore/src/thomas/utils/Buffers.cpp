#include "Buffers.h"
#include "..\Common.h"
#include "..\ThomasCore.h"

namespace thomas
{
	namespace utils
	{
		namespace buffers
		{
			Buffer::Buffer(void * data, size_t size, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag, size_t structureByteStride, D3D11_RESOURCE_MISC_FLAG miscFlag) : m_size(size), m_bindFlag(bindFlag)
			{
				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.ByteWidth = (uint32_t)size;
				bufferDesc.Usage = usageFlag; 
				bufferDesc.BindFlags = bindFlag;
				bufferDesc.CPUAccessFlags = usageFlag == DYNAMIC_BUFFER ? D3D11_CPU_ACCESS_WRITE : 0; //CPU if dynamic
				bufferDesc.MiscFlags = miscFlag;
				bufferDesc.StructureByteStride = (uint32_t)structureByteStride;
				
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = data;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				m_usageFlag = usageFlag;

				HRESULT result;

				if (data == nullptr)
					result = utils::D3D::Instance()->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
				else
					result = utils::D3D::Instance()->GetDevice()->CreateBuffer(&bufferDesc, &InitData, &m_buffer);


				if (result != S_OK)
				{
					LOG(result);
				}
				else
				{
#ifdef _DEBUG
					// Give debug names to all buffers.
					static uint32_t BUFF_INDEX = 0;
					std::string debugName("BUFF" + std::to_string(BUFF_INDEX++) +  ", SIZE: " + std::to_string(size));
					SetName(debugName);
#endif
				}
			}
			Buffer::~Buffer()
			{
				Release();
			}
			void Buffer::Release()
			{
				SAFE_RELEASE(m_buffer);
			}
			void Buffer::SetData(void * data, size_t size)
			{
				if (size == 0) return;
				if (size > m_size)
				{
					LOG("Cannot set buffer data. The data size is bigger than the buffer.");
					return;
				}
				if (m_usageFlag != DYNAMIC_BUFFER)
				{
					LOG("Cannot uppload data at runtime. Usage flag is DYNAMIC_BUFFER.");
					return;
				}

				D3D11_MAPPED_SUBRESOURCE resource;
				utils::D3D::Instance()->GetDeviceContextDeferred()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
				memcpy(resource.pData, data, size);
				utils::D3D::Instance()->GetDeviceContextDeferred()->Unmap(m_buffer, 0);
			}
			void Buffer::SetName(const char * name)
			{
#ifdef _DEBUG
				m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(name) - 1, name);
#endif
			}
			void Buffer::SetName(const std::string & name)
			{
#ifdef _DEBUG
				m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.length() - 1, name.c_str());
#endif
			}
			size_t Buffer::GetByteSize()
			{
				return m_size;
			}
			ID3D11Buffer * Buffer::GetBuffer()
			{
				return m_buffer;
			}
			VertexBuffer::VertexBuffer(void * data, size_t stride, size_t count, D3D11_USAGE usageFlag): 
				Buffer(data, stride*count, D3D11_BIND_VERTEX_BUFFER, usageFlag), m_stride((uint32_t)stride)
			{
			}
			uint32_t VertexBuffer::GetStride()
			{
				return m_stride;
			}
			IndexBuffer::IndexBuffer(void * data, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER) : 
				Buffer(data, sizeof(UINT) * count, D3D11_BIND_INDEX_BUFFER, usageFlag),
				m_indexCount((uint32_t)count)
			{

			}

			
			StructuredBuffer::StructuredBuffer(void * data, size_t stride, size_t count, D3D11_USAGE usageFlag, D3D11_BIND_FLAG bindFlag, D3D11_BUFFER_UAV_FLAG uavFlag) : 
				Buffer(data, count * stride, bindFlag, usageFlag, stride, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				m_hasSRV = false;
				m_hasUAV = false;
				HRESULT hr;

				if (bindFlag & D3D11_BIND_SHADER_RESOURCE)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;

					desc.Buffer.ElementWidth = (uint32_t)stride;
					desc.Buffer.ElementOffset = 0;
					desc.Buffer.FirstElement = 0;
					desc.Buffer.NumElements = (uint32_t)count;
					desc.Format = DXGI_FORMAT_UNKNOWN;
					desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

					hr = utils::D3D::Instance()->GetDevice()->CreateShaderResourceView(m_buffer, &desc, &m_srv);

					if (hr == S_OK)
						m_hasSRV = true;
				}
				if (bindFlag & D3D11_BIND_UNORDERED_ACCESS)
				{
					D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = uavFlag;
					uavDesc.Buffer.NumElements = (uint32_t)count;
					uavDesc.Format = DXGI_FORMAT_UNKNOWN;
					uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

					hr = utils::D3D::Instance()->GetDevice()->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_uav);

					if (hr == S_OK)
						m_hasUAV = true;
				}
				
			}
			

			void StructuredBuffer::Release()
			{
				if (m_hasSRV)
					SAFE_RELEASE(m_srv);
				if (m_hasUAV)
					SAFE_RELEASE(m_uav);
			}

			ID3D11ShaderResourceView * StructuredBuffer::GetSRV()
			{
				if (m_hasSRV)
					return m_srv;

				LOG("No availible srv");
				return nullptr;
			}

			ID3D11UnorderedAccessView * StructuredBuffer::GetUAV()
			{
				if (m_hasUAV)
					return m_uav;
				
				LOG("No availible uav");
				return nullptr;
			}

			ByteAddressBuffer::ByteAddressBuffer(size_t stride, size_t count, void* data, D3D11_BIND_FLAG bindFlags) : Buffer(data, count * stride, bindFlags, STATIC_BUFFER, stride, D3D11_RESOURCE_MISC_FLAG(D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS))
			{
				m_hasSRV = false;
				m_hasUAV = false;
				HRESULT hr;

				if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
					srvDesc.BufferEx.FirstElement = 0;
					srvDesc.BufferEx.NumElements = (uint32_t)count;
					srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
					srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;

					hr = utils::D3D::Instance()->GetDevice()->CreateShaderResourceView(m_buffer, &srvDesc, &m_srv);

					if (hr == S_OK)
						m_hasSRV = true;
				}
				if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
				{
					D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
					uavDesc.Buffer.FirstElement = 0;
					uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
					uavDesc.Buffer.NumElements = (uint32_t)count;
					uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

					hr = utils::D3D::Instance()->GetDevice()->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_uav);

					if (hr == S_OK)
						m_hasUAV = true;
				}
				

			}
			void ByteAddressBuffer::Release()
			{
				if (m_hasSRV)
					SAFE_RELEASE(m_srv);
				if (m_hasUAV)
					SAFE_RELEASE(m_uav);
			}
			ID3D11ShaderResourceView * ByteAddressBuffer::GetSRV()
			{
				if (m_hasSRV)
					return m_srv;

				LOG("No availible srv");
				return nullptr;
			}


			ID3D11UnorderedAccessView * ByteAddressBuffer::GetUAV()
			{
				if (m_hasUAV)
					return m_uav;

				LOG("No availible uav");
				return nullptr;
			}

}
	}
}