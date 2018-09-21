#include "Frame.h"
#include "../../resource/Material.h"

namespace thomas {

	namespace graphics {

		namespace render {



			bool MaterialSorter::operator()(resource::Material * mat1, resource::Material * mat2) const
			{
				if (mat1->m_renderQueue == mat2->m_renderQueue)
					return mat1->GetId() < mat2->GetId();
				else
					return mat1->m_renderQueue < mat2->m_renderQueue;
			}
			Frame::Frame(unsigned int allocSize)
				: m_queue(), m_alloc(allocSize)
			{
			}
			void Frame::clear()
			{
				m_queue.clear();
				m_alloc.clear();
			}
		}
	}
}