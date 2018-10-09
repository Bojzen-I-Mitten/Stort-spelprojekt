#include "IK_FABRIK_Constraint.h"
#include "../../../ThomasCore.h"
#include "../../../resource/MemoryAllocation.h"



namespace thomas {
	namespace graphics {
		namespace animation {

			IK_FABRIK_Constraint::IK_FABRIK_Constraint(const std::vector<LinkParameter>& link_chain)
				: m_chain(new LinkParameter[link_chain.size()]), m_num_link(link_chain.size())
			{
				std::memcpy(m_chain.get(), link_chain.data(), sizeof(LinkParameter) * m_num_link);
			}

			IK_FABRIK_Constraint::~IK_FABRIK_Constraint()
			{
			}

			void IK_FABRIK_Constraint::execute(Skeleton & skel, math::Matrix * objectPose, uint32_t boneInd)
			{
				float * d = reinterpret_cast<float*>(
					ThomasCore::Core().Memory()->stack(0).allocate(sizeof(float) * m_num_link, sizeof(float)));

			}

		}
	}
}