#pragma once
#include "D3D.h"
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
				Buffer(void* data, size_t size, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag, D3D11_RESOURCE_MISC_FLAG miscFlag = (D3D11_RESOURCE_MISC_FLAG)0, size_t structureByteStride = 0);
				template <typename T>
				Buffer(std::vector<T>& data, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usageFlag) : Buffer(name, data.data(), sizeof(data[0])*data.size(), bindFlag, usageFlag) {};
				virtual ~Buffer();
				void SetData(void* data, size_t size);

				template <typename T>
				void SetData(std::vector<T>& data) {SetData(data.data(), data.size() * sizeof(T));}

				size_t GetSize();
				ID3D11Buffer* GetBuffer();
			protected:
				D3D11_BIND_FLAG m_bindFlag;
				size_t m_size;
				ID3D11Buffer * m_buffer;
			};


			class VertexBuffer : public Buffer
			{
			public:
				VertexBuffer(void* data, size_t stride, size_t count, D3D11_USAGE usageFlag = STATIC_BUFFER);
				template <typename T>
				VertexBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : VertexBuffer(data.data(), sizeof(data[0]), data.size(), usageFlag) {};

				template <typename T>
				void SetData(std::vector<T>& data) 
				{
					if (sizeof(data[0]) != m_stride)
					{
						LOG("Cannot set vertex buffer data. new data stride differs from buffers stride");
						return;
					}

					Buffer::SetData(data.data(), data.size() * sizeof(data[0]));
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
				IndexBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : IndexBuffer(data.data(), data.size(), usageFlag) {};
			};

			class StructuredBuffer : public Buffer
			{
			public:
				StructuredBuffer(void* data, size_t stride, size_t count, D3D11_USAGE usageFlag);
				template <typename T>
				StructuredBuffer(std::vector<T>& data, D3D11_USAGE usageFlag = STATIC_BUFFER) : StructuredBuffer(name, data.data(), sizeof(data[0]), data.size(), usageFlag) {};
				~StructuredBuffer();

				ID3D11ShaderResourceView* GetSRV();
			private:
				ID3D11ShaderResourceView* m_resource;
			};
		}
	}
}