#pragma once
#include <vector>
#include "..\utils/d3d.h"
#include "..\Common.h"

namespace thomas
{
	namespace utils
	{
		namespace buffers
		{
#define STATIC_BUFFER D3D11_USAGE_DEFAULT
#define DYNAMIC_BUFFER D3D11_USAGE_DYNAMIC

			class Buffer
			{
			public:
				//Buffer(void* data, size_t size, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag);
				Buffer(void* data, size_t size, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag = STATIC_BUFFER, size_t structureByteStride = 0, D3D11_RESOURCE_MISC_FLAG miscFlag = (D3D11_RESOURCE_MISC_FLAG)0);
				template <typename T>
				Buffer(std::vector<T>& data, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag) : Buffer(data.data(), sizeof(T)*data.size(), bindFlag, usageFlag) {}

				virtual ~Buffer();
				virtual void Release();
				void SetData(void* data, size_t size);

				template <typename T>
				void SetData(std::vector<T>& data) {SetData(data.data(), data.size() * sizeof(T));}
				/* Give a debug name to the buffer.
				*/
				void SetName(const char* name);
				/* Give a debug name to the buffer.
				*/
				void SetName(const std::string& name);

				size_t GetByteSize();
				ID3D11Buffer* GetBuffer();
			protected:
				D3D11_BIND_FLAG m_bindFlag;
				D3D11_USAGE m_usageFlag;
				size_t m_size;
				ID3D11Buffer * m_buffer;
			};


			class VertexBuffer : public Buffer
			{
			public:
				VertexBuffer(void* data, size_t stride, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER);
				template <typename T>
				VertexBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : VertexBuffer(data.data(), sizeof(T), data.size(), usageFlag) {};

				template <typename T>
				void SetData(std::vector<T>& data) 
				{
					if (sizeof(T) != m_stride)
					{
						LOG("Cannot set vertex buffer data. new data stride differs from buffers stride");
						return;
					}

					Buffer::SetData(data.data(), data.size() * sizeof(T));
				}
				template <typename T>
				void SetData(T* data, uint32_t numVertex)
				{
					if (sizeof(T) != m_stride)
					{
						LOG("Cannot set vertex buffer data. new data stride differs from buffers stride");
						return;
					}

					Buffer::SetData(data, numVertex * sizeof(T));
				}

				size_t GetStride();
			private:
				size_t m_stride;
			};


			class IndexBuffer : public Buffer
			{
			public:
				IndexBuffer(void* data, size_t count, D3D11_USAGE usageFlag);
				template <typename T>
				IndexBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : 
					IndexBuffer(data.data(), data.size(), usageFlag)
				{};

				size_t IndexCount() { return m_indexCount; }

			private:
				size_t m_indexCount;
			};

			class StructuredBuffer : public Buffer
			{
			public:
				StructuredBuffer(void* data, size_t stride, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER, D3D11_BIND_FLAG bindFlag = D3D11_BIND_SHADER_RESOURCE, D3D11_BUFFER_UAV_FLAG uavFlag = (D3D11_BUFFER_UAV_FLAG)0);
				template <typename T>
				StructuredBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : StructuredBuffer(data.data(), sizeof(T), data.size(), usageFlag) {};

				void Release();


				ID3D11ShaderResourceView* GetSRV();
				ID3D11UnorderedAccessView* GetUAV();

			private:
				ID3D11ShaderResourceView* m_srv;
				bool m_hasSRV;
				ID3D11UnorderedAccessView* m_uav;
				bool m_hasUAV;
			};

			class ByteAddressBuffer : public Buffer
			{
			public:
				ByteAddressBuffer(size_t stride, size_t count, void* data = nullptr, D3D11_BIND_FLAG bindFlags = D3D11_BIND_UNORDERED_ACCESS);

				void Release();


				ID3D11ShaderResourceView* GetSRV();
				ID3D11UnorderedAccessView* GetUAV();
			private:
				ID3D11ShaderResourceView* m_srv;
				bool m_hasSRV;
				ID3D11UnorderedAccessView* m_uav;
				bool m_hasUAV;
			};
		}
	}
}