#include "IK_FABRIK_Constraint.h"
#include "../../../ThomasCore.h"
#include "../../../resource/MemoryAllocation.h"



namespace thomas {
	namespace graphics {
		namespace animation {

			IK_FABRIK_Constraint::IK_FABRIK_Constraint(uint32_t num_link)
				: m_chain(new LinkParameter[num_link]), m_num_link(num_link)
			{
			}
			IK_FABRIK_Constraint::IK_FABRIK_Constraint(const std::vector<LinkParameter>& link_chain)
				: IK_FABRIK_Constraint(link_chain.size())
			{
				std::memcpy(m_chain.get(), link_chain.data(), sizeof(LinkParameter) * m_num_link);
			}

			IK_FABRIK_Constraint::~IK_FABRIK_Constraint()
			{
			}

			void IK_FABRIK_Constraint::setLinkIndex(uint32_t chainIndex, uint32_t boneIndex) {
				assert(chainIndex < m_num_link);
				m_chain.get()[chainIndex].m_index = boneIndex;
			}
			math::Vector3 lerp(const math::Vector3& from, const math::Vector3& to, float amount)
			{
				return from * (1.f - amount) + to * amount;
			}

			/* FABRIK out of reach edge case solution.
			*/
			void FABRIK_unreachable(math::Vector3 target, float *d, math::Vector3*p, uint32_t num_link)
			{
				// Target is further away then the length of the chain
				for (uint32_t i = 0; i < num_link - 1; i++) {
					float r = math::Vector3::Distance(p[i], target);
					float lambda = d[i] / r;
					p[i+1] = lerp(p[i], target, lambda);
				}
			}
			/* FABRIK backward/forward iteration
			*/
			void FABRIK_iteration(math::Vector3 target, float *d, math::Vector3*p, uint32_t num_link)
			{
				// Target is within reach
				math::Vector3 b = p[0];
				float dif = math::Vector3::Distance(p[num_link - 1], target);
				// Iterate until a error tolerance is reached (or max. iterations)
				for (uint32_t iter = 0; dif > FABRIK_TOLERANCE && iter < MAX_FABRIK_ITER; iter++) {
					// Stage 1: Forward reaching
					uint32_t i = num_link - 1;
					p[i] = target;
					for(; i > 0;){												// Forward reaching loop
						i--;
						float r = math::Vector3::Distance(p[i], p[i+1]);		// Distance to next joint
						float lambda = d[i] / r;
						p[i] = lerp(p[i+1], p[i], lambda);						// Next iter. position
					}
					// Stage 2: Backward reaching
					*p = b;														// Reset root
					for (; i < num_link - 1; i++) {								// Backward reaching loop
						float r = math::Vector3::Distance(p[i], p[i + 1]);		// Distance to next joint
						float lambda = d[i] / r;
						p[i+1] = lerp(p[i], p[i + 1], lambda);					// Next iter. position
					}
					dif = math::Vector3::Distance(p[num_link-1], target);
				}
			}

			/* FABRIK IK solver
			*/
			void IK_FABRIK_Constraint::execute(Skeleton & skel, math::Matrix * objectPose, uint32_t boneInd)
			{
				// Stack alloc.
				float * d = reinterpret_cast<float*>(
					ThomasCore::Core().Memory()->stack(0).allocate(sizeof(float) * m_num_link * 4, sizeof(float)));
				math::Vector3 *p = reinterpret_cast<math::Vector3*>(d + m_num_link);
				// Calc. distance of each bone/link
				LinkParameter* chain = m_chain.get();
				float link_sum = 0.f;
				uint32_t i = 0;
				for (; i < m_num_link - 1; i++) {										// Find position of each bone
					p[i] = objectPose[chain[i].m_index].Translation();
					d[i] = math::Vector3::Distance(
						p[i], objectPose[chain[i+1].m_index].Translation());
					link_sum += d[i];
				}
				p[i] = objectPose[chain[i].m_index].Translation();						// End case
				// Distance from root to target
				float targetDist = math::Vector3::Distance(
					objectPose[m_chain.get()[0].m_index].Translation(), m_target);
				if (targetDist > link_sum)
					FABRIK_unreachable(m_target, d, p, m_num_link);
				else
					FABRIK_iteration(m_target, d, p, m_num_link);

				// Apply solution to chain
				for (i = 0; i < m_num_link - 1; i++) {
					math::Matrix pose = objectPose[(chain+i)->m_index];
					math::Quaternion q = math::getRotationTo(pose.Up(), p[i+1] - p[i]);	// Rotation difference from current pose to target
					pose.Translation(math::Vector3::Zero);								// Remove translation
					pose = pose * math::Matrix::CreateFromQuaternion(q);				// Rotate
					pose.Translation(p[i]);												// Apply new translation
					objectPose[(chain+i)->m_index] = pose;								// Set
				}
				objectPose[(chain + m_num_link - 1)->m_index].Translation(p[m_num_link-1]);
				// Clean stack
				ThomasCore::Core().Memory()->stack(0).deallocate(d);
			}

		}
	}
}