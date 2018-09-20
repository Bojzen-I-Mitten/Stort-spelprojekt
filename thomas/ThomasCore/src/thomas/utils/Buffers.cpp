#include "Buffers.h"
#include "..\Common.h"

namespace thomas
{
	namespace utils
	{
		namespace buffers
		{
			Buffer::Buffer(const char* name, void * data, size_t size, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag = STATIC_BUFFER, D3D11_RESOURCE_MISC_FLAG miscFlag, size_t structureByteStride) : m_size(size), m_bindFlag(bindFlag)
			{
				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.ByteWidth = size;
				bufferDesc.Usage = usageFlag; //TODO: Maybe dynamic for map/unmap
				bufferDesc.BindFlags = bindFlag;
				bufferDesc.CPUAccessFlags = usageFlag == DYNAMIC_BUFFER ? D3D11_CPU_ACCESS_WRITE : 0; //CPU if dynamic
				bufferDesc.MiscFlags = miscFlag;
				bufferDesc.StructureByteStride = structureByteStride;
				
					

				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = data;
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;

				HRESULT result;
				if (data == nullptr)
					result = ThomasCore::GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
				else
					result = ThomasCore::GetDevice()->CreateBuffer(&bufferDesc, &InitData, &m_buffer);

				m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name) - 1, name);

				if (result != S_OK)
					LOG(result);
			}
			Buffer::~Buffer()
			{
				SAFE_RELEASE(m_buffer);
			}

			void Buffer::SetData(void * data, size_t size)
			{
				if (size == 0) return;
				if (size > m_size)
				{
					LOG("Cannot set buffer data. the data size is bigger than the buffer.");
					return;
				}

				D3D11_MAPPED_SUBRESOURCE resource;
				ThomasCore::GetDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
				memcpy(resource.pData, data, size);
				ThomasCore::GetDeviceContext()->Unmap(m_buffer, 0);
			}
			size_t Buffer::GetSize()
			{
				return m_size;
			}
			ID3D11Buffer * Buffer::GetBuffer()
			{
				return m_buffer;
			}
			VertexBuffer::VertexBuffer(const char* name, void * data, size_t stride, size_t count, D3D11_USAGE usageFlag): Buffer(name, data, stride*count, D3D11_BIND_VERTEX_BUFFER, usageFlag), m_stride(stride)
			{
			}
			size_t VertexBuffer::GetStride()
			{
				return m_stride;
			}
			IndexBuffer::IndexBuffer(const char* name, void * data, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER) : Buffer(name, data, sizeof(UINT) * count, D3D11_BIND_INDEX_BUFFER, usageFlag)
			{
			}
			StructuredBuffer::StructuredBuffer(const char* name, void * data, size_t stride, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER) : Buffer(name, data, count * stride, D3D11_BIND_SHADER_RESOURCE, usageFlag, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, stride)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC desc;
				
				desc.Buffer.ElementWidth = stride;
				desc.Buffer.ElementOffset = 0;
				desc.Buffer.FirstElement = 0;
				desc.Buffer.NumElements = count;
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				
				ThomasCore::GetDevice()->CreateShaderResourceView(m_buffer, &desc, &m_resource);
				
				ThomasCore::SetDebugObjectName(m_resource, "BufferResource");
			}

			StructuredBuffer::~StructuredBuffer()
			{
				m_resource->Release();
			}

			ID3D11ShaderResourceView * StructuredBuffer::GetSRV()
			{
				return m_resource;
			}

		}
	}
}