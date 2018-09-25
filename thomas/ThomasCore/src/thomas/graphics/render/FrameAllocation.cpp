#include "FrameAllocation.h"


namespace thomas{

	namespace graphics {

		namespace render {

			FrameAllocation::FrameAllocation(unsigned int size, unsigned int numDataBytes)
				: m_alloc()
			{
				m_alloc.reserve(size);
				m_dataAlloc = new char[numDataBytes];
				m_dataTotalBytes = numDataBytes;
			}
			FrameAllocation::~FrameAllocation() {
				clear();
			}

			const resource::shaderproperty::ShaderPropertyStatic * FrameAllocation::allocate(const resource::shaderproperty::ShaderPropertyStatic * alloc)
			{
				if (m_alloc.size() == m_alloc.capacity() && canAllocate(alloc->m_dataSize))
					return  NULL; // Should be dummy
				m_alloc.push_back(*alloc);
				alloc = &m_alloc.back();
				copy(m_alloc.back());

				return &m_alloc.back();
			}

			const resource::shaderproperty::ShaderPropertyStatic * FrameAllocation::allocate(const resource::shaderproperty::ShaderPropertyStatic * alloc, unsigned int num)
			{
				uint32_t totalBytes = 0;
				for (unsigned int i = 0; i < num; i++)
					totalBytes += alloc[i].m_dataSize;

				if (m_alloc.size() + num  >= m_alloc.capacity() && canAllocate(totalBytes))
					return  NULL;			// Should be dummy
				// Create frame data copy
				unsigned int index = m_alloc.size();
				for (unsigned int i = 0; i < num; i++) {
					m_alloc.push_back(alloc[i]);
					copy(m_alloc[i]);
				}
				return &m_alloc[index];
			}

			void FrameAllocation::clear()
			{
				m_allocatedBytes = 0;
				m_alloc.clear();
			}

			FrameAllocation & FrameAllocation::operator=(FrameAllocation && move)
			{
				if (this == &move) return *this;
				m_alloc = std::move(move.m_alloc);
				return *this;
			}

			bool FrameAllocation::canAllocate(uint32_t bytes)
			{
				return m_allocatedBytes + bytes < m_dataTotalBytes;
			}

			void* FrameAllocation::allocData(uint32_t off) {
				void *ptr = reinterpret_cast<char*>(m_dataAlloc) + m_allocatedBytes;
				m_allocatedBytes += off;
				return ptr;
			}

			void FrameAllocation::copy(resource::shaderproperty::ShaderPropertyStruct & info)
			{
				void * ptr = allocData(info.m_dataSize);
				std::memcpy(ptr, info.m_data, info.m_dataSize);
				info.m_data = ptr;
			}

		}
	}
}