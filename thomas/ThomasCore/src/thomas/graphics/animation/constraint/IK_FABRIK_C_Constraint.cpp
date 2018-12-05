#include "IK_FABRIK_C_Constraint.h"
#include "../../../ThomasCore.h"
#include "../../../resource/MemoryAllocation.h"
#include "../../../editor/gizmos/Gizmos.h"
#include "../data/Skeleton.h"

#define IK_DRAW

namespace thomas {
	namespace graphics {
		namespace animation {

			IK_FABRIK_C_Constraint::IK_FABRIK_C_Constraint(uint32_t num_link)
				: m_target(), m_targetOrient(), m_weight(1.f), m_orientationWeight(1.f),
					m_chain(new LinkParameter[num_link]), m_num_link(num_link)
			{
			}
			IK_FABRIK_C_Constraint::IK_FABRIK_C_Constraint(const std::vector<LinkParameter>& link_chain)
				: IK_FABRIK_C_Constraint((uint32_t)link_chain.size())
			{
				std::memcpy(m_chain.get(), link_chain.data(), sizeof(LinkParameter) * m_num_link);
			}

			IK_FABRIK_C_Constraint::~IK_FABRIK_C_Constraint()
			{
			}

			void IK_FABRIK_C_Constraint::setLinkAtIndex(uint32_t chainIndex, LinkParameter param) {
				assert(chainIndex < m_num_link);
				m_chain.get()[chainIndex] = param;
			}

			/* FABRIK out of reach edge case solution.
			*/
			void IK_FABRIK_C_Constraint::FABRIK_unreachable(math::Vector3 target, float *d, math::Vector3*p, uint32_t num_link)
			{
				// Target is further away then the length of the chain
				for (uint32_t i = 0; i < num_link - 1; i++) {
					float r = math::Vector3::Distance(p[i], target);
					float lambda = d[i] / r;
					p[i+1] = math::lerp(p[i], target, lambda);
				}
			}
			constexpr float limit_bend = math::PI / 2 - 0.01f * math::PI;
			constexpr float limit_twist = math::PI / 4;

			void IK_FABRIK_C_Constraint::solve_constraint_backward_iter(math::Vector3 *p_c, math::Matrix *c_orient, float bone_len)
			{
				math::Matrix m_ii = *(c_orient + 1);
				math::Vector3 p_i = *p_c;
				math::Vector3 p_ii = *(p_c + 1);
				math::Vector3 d_i = p_i - p_ii;
				math::Vector2 q_i;										// Project on to XZ plane of T_i+1
				q_i.x = d_i.Dot(m_ii.Right());						
				q_i.y = d_i.Dot(m_ii.Backward());
				float b = q_i.Length();
				float a = d_i.Dot(-m_ii.Up());							// Find distances to Y axis on T_i+1
				float c = std::fabs(a) * std::tanf(limit_bend);
				float l_proj = b - c;									// Calc. distance on XZ from p_i -> angle limit
				q_i *= l_proj;
				math::Vector3 proj_diff = q_i.x * m_ii.Right() + q_i.y * m_ii.Backward();			// Vector from p_in -> p_i
				math::Vector3 p_n = (bone_len / std::sqrtf(a*a + c * c)) * (p_i - proj_diff);		// Calc. p_in (normalized)
				if (a < 0.f)																		// If Point is infront of the matrix (should be behind)
				{
					p_n += 2 * a * m_ii.Up();														// Mirror result over T_i+1 XZ plane (always apply)
					*p_c = p_n; // Update p_i
				}
				else if (l_proj > 0.f)																// Don't apply if b < c (inside angle limit)
					*p_c = p_n; // Update p_i
				// else			// Do nothing

				// Re-calculate orientation
				d_i = (*p_c - p_ii) / bone_len;
				*c_orient = *c_orient * math::getMatrixRotationTo(c_orient->Up(), d_i);
			}
			void IK_FABRIK_C_Constraint::solve_constraint_forward_iter(math::Vector3 *p_c, math::Matrix *c_orient, float bone_len)
			{
				math::Matrix m_i = c_orient[-1];
				math::Vector3 p_i = p_c[1];
				math::Vector3 d_i = p_i - p_c[0];
				math::Vector2 q_i;										// Project on to XZ plane of T_i+1
				q_i.x = d_i.Dot(m_i.Right());
				q_i.y = d_i.Dot(m_i.Backward());
				float b = q_i.Length();
				float a = d_i.Dot(m_i.Up());							// Find distances to Y axis on T_i+1
				float c = std::fabs(a) * std::tanf(limit_bend);
				float l_proj = b - c;									// Calc. distance on XZ from p_i -> angle limit
				q_i *= l_proj / b;
				math::Vector3 proj_diff = q_i.x * m_i.Right() + q_i.y * m_i.Backward();				// Vector from p_in -> p_i
				math::Vector3 p_n = (d_i - proj_diff);												// Calc. p_in (non-normalized)
				if (a < 0.f)																		// If Point is infront of the matrix (should be behind)
					p_n -= 2 * a * m_i.Up();														// Mirror result over T_i+1 XZ plane (always apply)
				p_n = (bone_len / std::sqrtf(a*a + c * c)) * p_n;									// Make offset distance == bone length
				if (a > 0.f && l_proj <= 0.f)														// Don't apply if b < c (inside angle limit)
					p_n = p_i;
				else
					p_n = p_c[0] + p_n;																// Offset from prev.
				p_c[1] = p_n;
				// Re-calculate orientation
				d_i = (p_c[1] - p_c[0]) / bone_len;													// new forward orient
				c_orient[0] = c_orient[0] * math::getMatrixRotationTo(c_orient[0].Up(), d_i);		// Rotate prev.->new 
			}
			

			/* FABRIK backward/forward iteration
			*/
			void IK_FABRIK_C_Constraint::FABRIK_iteration(math::Vector3 target, float *len, math::Vector3*p, math::Matrix *orient, uint32_t num_link)
			{
				// Target is within reach
				math::Vector3 p_init = p[0];
				math::Matrix orient_init = orient[0];
				float dif = math::Vector3::Distance(p[num_link - 1], target);
				// Iterate until a error tolerance is reached (or max. iterations)
				for (uint32_t iter = 0; dif > FABRIK_TOLERANCE && iter < MAX_FABRIK_ITER; iter++) {
					// Stage 1: Forward reaching
					uint32_t i = num_link - 1;
					p[i] = target;
					for(; i > 0;){												// Forward reaching loop
						i--;
						float r = math::Vector3::Distance(p[i], p[i+1]);		// Distance to next joint
						float lambda = len[i] / r;
						p[i] = math::lerp(p[i+1], p[i], lambda);				// Next iter. position

						//solve_constraint_backward_iter(p + i, orient + i, len[i]);
					}
					// Stage 2: Backward reaching
					*p = p_init;												// Reset root
					*orient = orient_init;
					for (; i < num_link - 1; i++) {								// Backward reaching loop
						float r = math::Vector3::Distance(p[i], p[i + 1]);		// Distance to next joint
						float lambda = len[i] / r;
						p[i+1] = math::lerp(p[i], p[i + 1], lambda);			// Next iter. position

						if(i > 0)
							solve_constraint_forward_iter(p + i, orient + i, len[i]);
						else {
							math::Vector3 d_i = (p[i+1] - p[i]) / len[i];								// new forward orient
							orient[i] = orient[i] * math::getMatrixRotationTo(orient[i].Up(), d_i);		// Rotate prev.->new 
						}
					}
					dif = math::Vector3::Distance(p[num_link-1], target);
				}
			}

			

			/* FABRIK IK solver
			*/
			void IK_FABRIK_C_Constraint::execute(Skeleton & skel, math::Matrix * objectPose, TransformComponents* comp, uint32_t boneInd)
			{
				assert(m_num_link > 1);
				if (m_weight < math::EPSILON)
					return; // Do nothing

				// Stack alloc.
				uint32_t num_float_alloc = m_num_link * (3 + 1 + 16);	// Trans, len, rot mat
				void * alloc = ThomasCore::Core().Memory()->stack(0).allocate(sizeof(float) * num_float_alloc, sizeof(float));
				float *len = reinterpret_cast<float*>(alloc);
				math::Vector3 *p = reinterpret_cast<math::Vector3*>(len + m_num_link);
				math::Matrix *orient = reinterpret_cast<math::Matrix*>(p + m_num_link);



				// Calc. distance of each bone/link
				LinkParameter* chain = m_chain.get();
				float linkLengthSum = 0.f;
				uint32_t i = 0;
				for (; i < m_num_link - 1; i++) {												// Find position of each bone
					p[i] = objectPose[chain[i].m_index].Translation();							// Bone joint position (initial)
					orient[i] = math::extractRotation(objectPose[chain[i].m_index]);			// Get matrix orientation
					len[i] = math::Vector3::Distance(		
						p[i], objectPose[chain[i+1].m_index].Translation());					// Bone length
					linkLengthSum += len[i];													// Sum chain length
				}
				math::Quaternion targetO = math::Quaternion::CreateFromRotationMatrix(			// Calc. target orientation
					math::extractRotation(objectPose[chain[i].m_index]));
				targetO = math::Quaternion::Slerp(targetO, m_targetOrient, m_orientationWeight);// End cases
				p[i] = objectPose[chain[i].m_index].Translation();								
				math::Vector3 targetP = math::Vector3::Lerp(p[i], m_target, m_weight);			// Calc. target point
				orient[i] = math::Matrix::CreateFromQuaternion(targetO);						// --
				// Distance from root to target
				float targetDist = math::Vector3::Distance(
					objectPose[m_chain.get()[0].m_index].Translation(), targetP);
				if (targetDist > linkLengthSum)
					FABRIK_unreachable(targetP, len, p, m_num_link);
				else
					FABRIK_iteration(targetP, len, p, orient, m_num_link);
				math::Vector3 trans;
				math::Matrix pose;
				// Apply solution to chain
				for (i = 0; i < m_num_link - 1; i++) {
					pose = objectPose[(chain+i)->m_index];
					trans = pose.Translation();
					pose.Translation(math::Vector3::Zero);										// Remove translation
					pose = pose * math::getMatrixRotationTo(pose.Up(), p[i + 1] - p[i]);		// Rotate
					pose.Translation(p[i]);														// Apply new translation
					objectPose[(chain+i)->m_index] = pose;										// Set
				}
				// End case, apply target orient
				math::Vector3 up = math::Vector3::Transform(math::Vector3::Up, targetO);
				math::Vector3 right = math::Vector3::Transform(math::Vector3::Right, targetO);
				pose = objectPose[(chain + m_num_link - 1)->m_index];
				trans = pose.Translation();
				pose.Translation(math::Vector3::Zero);								// Remove translation
				pose = pose * math::getMatrixRotationTo(pose.Up(), up);				// Rotate transform to y
				pose = pose * math::getMatrixRotationTo(pose.Right(), right);		// Rotate transform to x
				pose.Translation(p[m_num_link - 1]);								// Apply new translation
				objectPose[(chain + m_num_link - 1)->m_index] = pose;

				const float GIZMO_LEN = 0.05f;
#ifdef _EDITOR
#ifdef IK_DRAW
				for (i = 0; i < m_num_link; i++) {
					editor::Gizmos::Gizmo().SetColor(math::Color(0, 1.f, 0.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Up()) * GIZMO_LEN);
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 0.f, 0.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Right()) * GIZMO_LEN);
					editor::Gizmos::Gizmo().SetColor(math::Color(0.f, 0.f, 1.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Backward()) * GIZMO_LEN);
				}
#endif
#endif
				// Clean stack
				ThomasCore::Core().Memory()->stack(0).deallocate(alloc);
			}
			bool IK_FABRIK_C_Constraint::apply(Skeleton & skel, uint32_t boneInd)
			{
				if (m_num_link < 2)
					return false;	// No links in bone chain
				// Calculate bone chain length
				float sum = 0.f;
				math::Vector3 v = skel.getBone(0)._bindPose.Translation();
				int validCount = m_chain.get()[0].m_index < skel.getNumBones();;
				for (uint32_t i = 1; i < m_num_link; i++) {
					math::Vector3 vn = skel.getBone(i)._bindPose.Translation();
					sum += math::Vector3::Distance(v, vn);
					v = vn;
					validCount += m_chain.get()[i].m_index < skel.getNumBones();
				}
				m_chainLength = sum;
				return validCount == m_num_link;
			}
			float IK_FABRIK_C_Constraint::getChainLength()
			{
				return m_chainLength;
			}
			uint32_t IK_FABRIK_C_Constraint::getSrcBoneIndex()
			{
				return m_chain.get()[0].m_index;
			}
		}
	}
}