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
				for (thomas::resource::shaderproperty::ShaderProperty * p : m_alloc)
					delete p;
			}

		}
	}
}