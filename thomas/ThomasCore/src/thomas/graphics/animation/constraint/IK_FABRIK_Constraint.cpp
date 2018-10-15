#include "IK_FABRIK_Constraint.h"
#include "../../../ThomasCore.h"
#include "../../../resource/MemoryAllocation.h"
#include "../../../editor/gizmos/Gizmos.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			IK_FABRIK_Constraint::IK_FABRIK_Constraint(uint32_t num_link)
				: m_target(), m_targetOrient(), m_weight(1.f),
					m_chain(new LinkParameter[num_link]), m_num_link(num_link)
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

			void IK_FABRIK_Constraint::setLinkAtIndex(uint32_t chainIndex, LinkParameter param) {
				assert(chainIndex < m_num_link);
				m_chain.get()[chainIndex] = param;
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

			math::Matrix weightRotationBetween(math::Vector3 from, math::Vector3 dest, float weight)
			{
				assert(weight >= 0.f && weight <= 1.f);
				from.Normalize();
				dest.Normalize();

				float c = from.Dot(dest);
				if (c >= 1.0f) // Vectors are parallel	(identical)
					return math::Matrix::Identity;
				if (c < (1e-6f - 1.0f)) // Vectors are parallel (opposite)
				{
					// Generate an axis
					math::Vector3 axis = math::Vector3::UnitX.Cross(from);
					if (axis.LengthSquared() < 0.0001f) // Pick another if colinear
						axis = math::Vector3::UnitY.Cross(from);
					axis.Normalize();
					return math::Matrix::CreateFromAxisAngle(axis, math::PI * weight);
				}
				else {

					// Credit goes to: Jur Van den Berg 
					// https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d/476311#476311
					math::Vector3 v = from.Cross(dest);
					float a = std::acosf(c) * weight + 1.f - weight; // Lerp angle 
					float s = 1.f / (1.f + std::cosf(a));

					math::Matrix vx(
						0.f, v.z, -v.y, 0.f,
						-v.z, 0.f, v.x, 0.f,
						v.y, -v.x, 0.f, 0.f,
						0.f, 0.f, 0.f, 0.f);
					vx = vx + (vx * vx) * s; // Simplify square calc? ...
					// Add identity
					vx._11 += 1.f;
					vx._22 += 1.f;
					vx._33 += 1.f;
					vx._44 += 1.f;
					return vx;
				}
			}
			/* FABRIK IK solver
			*/
			void IK_FABRIK_Constraint::execute(Skeleton & skel, math::Matrix * objectPose, TransformComponents* comp, uint32_t boneInd)
			{
				assert(m_num_link > 1);
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
				math::Vector3 trans;
				math::Matrix pose;
				// Apply solution to chain
				for (i = 0; i < m_num_link - 1; i++) {
					pose = objectPose[(chain+i)->m_index];
					trans = pose.Translation();
					pose.Translation(math::Vector3::Zero);										// Remove translation
					pose = pose * weightRotationBetween(pose.Up(), p[i + 1] - p[i], m_weight);	// Rotate
					pose.Translation(lerp(trans, p[i], m_weight));								// Apply new translation
					objectPose[(chain+i)->m_index] = pose;										// Set
				}
				math::Vector3 up = math::Vector3::Transform(math::Vector3::Up, m_targetOrient);
				math::Vector3 right = math::Vector3::Transform(math::Vector3::Right, m_targetOrient);
				pose = objectPose[(chain + m_num_link - 1)->m_index];
				trans = pose.Translation();
				pose.Translation(math::Vector3::Zero);											// Remove translation
				pose = pose * weightRotationBetween(pose.Up(), up, m_weight);					// Rotate transform to y
				pose = pose * weightRotationBetween(pose.Right(), right, m_weight);				// Rotate transform to x
				pose.Translation(lerp(trans, p[m_num_link - 1], m_weight));								// Apply new translation
				objectPose[(chain + m_num_link - 1)->m_index] = pose;

				const float GIZMO_LEN = 0.05f;
#ifdef _EDITOR
				for (i = 0; i < m_num_link; i++) {
					editor::Gizmos::Gizmo().SetColor(math::Color(0, 1.f, 0.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Up()) * GIZMO_LEN);
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 0.f, 0.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Right()) * GIZMO_LEN);
					editor::Gizmos::Gizmo().SetColor(math::Color(0.f, 0.f, 1.f));
					editor::Gizmos::Gizmo().DrawLine(p[i], p[i] + math::Normalize(objectPose[(chain + i)->m_index].Forward()) * GIZMO_LEN);
				}
#endif
				// Clean stack
				ThomasCore::Core().Memory()->stack(0).deallocate(d);
			}
		}
	}
}