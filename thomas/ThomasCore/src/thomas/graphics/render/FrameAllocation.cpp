#include "FrameAllocation.h"


namespace thomas{

	namespace graphics {

		namespace render {

			FrameAllocation::FrameAllocation(unsigned int size) 
				: m_alloc()
			{
				m_alloc.reserve(size);
			}
			FrameAllocation::~FrameAllocation() {
				clear();
			}

			resource::shaderproperty::ShaderProperty ** FrameAllocation::allocate(resource::shaderproperty::ShaderProperty * alloc)
			{
				if (m_alloc.size() == m_alloc.capacity())
					return  &m_alloc.front(); // Should be dummy
				m_alloc.push_back(alloc);
				return &m_alloc.back();
			}

			resource::shaderproperty::ShaderProperty ** FrameAllocation::allocate(resource::shaderproperty::ShaderProperty ** alloc, unsigned int num)
			{
				if (m_alloc.size() + num  >= m_alloc.capacity())
					return  &m_alloc.front();			// Should be dummy
				unsigned int index = m_alloc.size();
				for(unsigned int i = 0; i < num; i++)	// Make own struct for memcpy
					m_alloc.push_back(alloc[i]);
				return &m_alloc[index];
			}

			bool FrameAllocation::reserve(unsigned int num, resource::shaderproperty::ShaderProperty** &ptr)
			{
				if (m_alloc.size() + num >= m_alloc.capacity())
					return true;
				// Reserve ptr
				unsigned int at = m_alloc.size();
				for (unsigned int i = 0; i < num; i++)	// Make own struct for memcpy
					m_alloc.push_back(nullptr);
				ptr = &m_alloc[at];
				return false;
			}

			void FrameAllocation::clear()
			{
				for (thomas::resource::shaderproperty::ShaderProperty * p : m_alloc)
					delete p;
				m_alloc.clear();
			}

			FrameAllocation & FrameAllocation::operator=(FrameAllocation && move)
			{
				if (this == &move) return *this;
				m_alloc = std::move(move.m_alloc);
				return *this;
			}

		}
	}
}