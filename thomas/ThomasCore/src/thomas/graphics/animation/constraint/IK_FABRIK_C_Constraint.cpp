#include "IK_FABRIK_C_Constraint.h"
#include "../../../ThomasCore.h"
#include "../../../resource/MemoryAllocation.h"
#include "../../../editor/gizmos/Gizmos.h"
#include "../data/Skeleton.h"

#define IK_DRAW
//#define DRAW_FORWARD_CONSTRAINT
//#define DRAW_BACKWARD_CONSTRAINT

namespace thomas {
	namespace graphics {
		namespace animation {

			IK_FABRIK_C_Constraint::IK_FABRIK_C_Constraint(uint32_t num_link)
				: m_target(), m_targetOrient(), m_weight(1.f), m_orientationWeight(1.f),
					m_chain(new uint32_t[num_link]), m_joint(new JointInfo[num_link]), m_num_link(num_link)
			{
				for (size_t i = 0; i < m_num_link; i++)
					m_joint.get()[i] = JointInfo();
			}
			IK_FABRIK_C_Constraint::IK_FABRIK_C_Constraint(const std::vector<LinkParameter>& link_chain)
				: IK_FABRIK_C_Constraint((uint32_t)link_chain.size())
			{
				for (size_t i = 0; i < link_chain.size(); i++)
					setLinkAtIndex(i, link_chain[i]);
			}

			IK_FABRIK_C_Constraint::~IK_FABRIK_C_Constraint()
			{
			}

			void IK_FABRIK_C_Constraint::setLinkAtIndex(uint32_t chainIndex, LinkParameter param) {
				assert(chainIndex < m_num_link);
				m_chain.get()[chainIndex] = param.m_index;
				setJointAtIndex(chainIndex, param.m_jointInfo);
			}

			void IK_FABRIK_C_Constraint::setJointAtIndex(uint32_t chainIndex, JointParams info)
			{
				assert(chainIndex < m_num_link);
				// Set joint limits
				m_joint.get()[chainIndex].limit_bend = info.limit_bend;
				m_joint.get()[chainIndex].limit_twist = info.limit_twist;
				m_joint.get()[chainIndex].orient_offset = info.orientation;
				m_joint.get()[chainIndex].joint_type = info.joint_type;
				m_joint.get()[chainIndex].refreshOrient();
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
			math::Vector3 IK_FABRIK_C_Constraint::ballJointConstraint(math::Matrix &m_i, math::Vector3 &p_o, math::Vector3 &p_t, float limit_bend, float bone_len)
			{
				math::Vector3 d_i = p_t - p_o;
				math::Vector2 q_i;										// Project on to XZ plane of T_i+1
				q_i.x = d_i.Dot(m_i.Right());
				q_i.y = d_i.Dot(m_i.Backward());
				float b = q_i.Length();
				float a = d_i.Dot(m_i.Up());							// Find distances to Y axis on T_i+1
				float c = std::fabs(a) * std::tanf(limit_bend);
				float l_proj = b - c;									// Calc. distance on XZ from boundary -> p_t
				q_i *= l_proj / b;
				math::Vector3 proj_diff = q_i.x * m_i.Right() + q_i.y * m_i.Backward();				// Vector from p_in -> p_t (edge to point)
				math::Vector3 p_n = (d_i - proj_diff);												// Calc. p_in (non-normalized)
				if (a < 0.f)																		// If Point is infront of the matrix (should be behind)
					p_n -= 2 * a * m_i.Up();														// Mirror result over T_i+1 XZ plane (always apply)
				p_n = (bone_len / std::sqrtf(a*a + c * c)) * p_n;									// Make offset distance == bone length
				if (a >= 0.f && l_proj <= 0.f)														// Don't apply if b < c (inside angle limit)
					p_n = p_t;
				else
					p_n = p_o + p_n;																// Offset from prev.
				return p_n;
			}
#define ASSERT_DIFF(v, v1) assert(std::fabs(v.x - v1.x) +  std::fabs(v.y - v1.y) + std::fabs(v.z - v1.z) > math::EPSILON)
			math::Vector3 IK_FABRIK_C_Constraint::swingJointConstraint(math::Matrix &m_i, math::Vector3 &p_o, math::Vector3 &p_t, float limit_bend, float bone_len)
			{
				math::Vector3 d_i = p_t - p_o;							// Project on YZ
				math::Vector3 proj_YZ = d_i.Dot(m_i.Right()) / m_i.Right().LengthSquared() * m_i.Right();
				d_i = d_i  - proj_YZ;

				float q_i = d_i.Dot(m_i.Backward());
				float b = std::fabs(q_i);															// Project on to XZ plane of T_i+1
				float a = d_i.Dot(m_i.Up());														// Find distances to Y axis on T_i+1
				float c = std::fabs(a) * std::tanf(limit_bend);
				float l_proj = b - c;																// Calc. distance on XZ from boundary -> p_t
				q_i *= l_proj / b;
				math::Vector3 proj_diff = q_i * m_i.Backward();										// Vector from p_in -> p_t (edge to point)
				math::Vector3 p_n = (d_i - proj_diff);												// Calc. p_in (non-normalized)
				if (a < 0.f)																		// If Point is infront of the matrix (should be behind)
					p_n -= 2 * a * m_i.Up();														// Mirror result over T_i+1 XZ plane (always apply)
				//p_n = (bone_len / std::sqrtf(a*a + c * c)) * p_n;									// Make offset distance == bone length
				if (a >= 0.f && l_proj <= 0.f)														// Use projected FABRIK result if b < c (inside angle limit)
					p_n = p_o + d_i * (bone_len / d_i.Length());
				else
					p_n = p_o + p_n * (bone_len / p_n.Length());									// Offset from p_o (and distance == bone length)
				ASSERT_DIFF(p_n, p_o);
				return p_n;
			}
			
			void IK_FABRIK_C_Constraint::solve_constraint_backward_iter(uint32_t index, math::Vector3 *p_c, math::Matrix *c_orient, float bone_len)
			{
				JointInfo& joint = m_joint.get()[index];
				JointInfo& joint_ii = m_joint.get()[index+1];
				/* Define the joint and orient it in relation to bone i, 
				 * joint_ii is required as it constains the transforms from i->ii and ii->i.
				*/
				math::Matrix m_ii = joint_ii.backwardOrient * c_orient[1];
				// Flip Y axis, basis is now facing opposite direction of the joint.
				math::invertY(m_ii);	
				math::Vector3 p_o = p_c[1];
				// Apply constraint
				math::Vector3 p_n;
				if(joint.joint_type)
					p_n = swingJointConstraint(m_ii, p_o, *p_c, joint.limit_bend, bone_len);
				else
					p_n = ballJointConstraint(m_ii, p_o, *p_c, joint.limit_bend, bone_len);
				// Re-calculate orientation
				math::Vector3 d_i = (p_o - p_n) / bone_len;
				p_c[0] = p_n;																		// New forward orient
				c_orient[0] = c_orient[0] * math::getMatrixRotationTo(c_orient[0].Up(), d_i);		// Rotate prev.->new
#ifdef DRAW_BACKWARD_CONSTRAINT
				{
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 1.f, 0.f));
					//editor::Gizmos::Gizmo().DrawLine(p_o, *p_c);					// FABRIK output line
					editor::Gizmos::Gizmo().DrawRay(p_o, m_ii.Up(), 0.1f);			// Joint orientation UP
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 0.f, 0.f));	
					editor::Gizmos::Gizmo().DrawLine(p_o, p_n);						// Constrained line
				}
#endif
			}
			void IK_FABRIK_C_Constraint::solve_constraint_forward_iter(uint32_t index, math::Vector3 *p_c, math::Matrix *c_orient, float bone_len)
			{
				JointInfo& joint = m_joint.get()[index];
				math::Matrix m_i = joint.orientation * c_orient[-1];
				math::Vector3 p_o = p_c[0];
				// Apply constraint
				math::Vector3 p_n;
				if (joint.joint_type)
					p_n = swingJointConstraint(m_i, p_o, p_c[1], joint.limit_bend, bone_len);
				else
					p_n = ballJointConstraint(m_i, p_o, p_c[1], joint.limit_bend, bone_len);
				// Re-calculate orientation
				math::Vector3 d_i = (p_n - p_o) / bone_len;
				p_c[1] = p_n;																		// New forward orient
				c_orient[0] = c_orient[0] * math::getMatrixRotationTo(c_orient[0].Up(), d_i);		// Rotate prev.->new
#ifdef DRAW_FORWARD_CONSTRAINT
				{
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 1.f, 0.f));
					//editor::Gizmos::Gizmo().DrawLine(p_o, *p_c);					// FABRIK output line
					editor::Gizmos::Gizmo().DrawRay(p_o, m_i.Up(), 0.1f);			// Joint orientation UP
					editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 0.f, 0.f));
					editor::Gizmos::Gizmo().DrawLine(p_o, p_n);						// Constrained line
				}
#endif
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
					for(; i > 0;){														// Forward reaching loop
						i--;
						math::Vector3 bonePoint = p[i];
						if(i < num_link - 1)
							bonePoint += math::Vector3(0, 0, math::EPSILON-len[i] * (MAX_FABRIK_ITER - iter) / MAX_FABRIK_ITER);
						float r = math::Vector3::Distance(bonePoint, p[i + 1]);				// Distance to next joint
						float lambda = len[i] / r;
						p[i] = math::lerp(p[i + 1], bonePoint, lambda);						// Next iter. position

						solve_constraint_backward_iter(i, p + i, orient + i, len[i]);
						/*
						float r = math::Vector3::Distance(p[i], p[i+1]);				// Distance to next joint
						float lambda = len[i] / r;
						p[i] = math::lerp(p[i+1], p[i], lambda);						// Next iter. position

						solve_constraint_backward_iter(i, p + i, orient + i, len[i]);
						*/
					}
					// Stage 2: Backward reaching
					*p = p_init;														// Reset root
					*orient = orient_init;
					for (; i < num_link - 1; i++) {										// Backward reaching loop
						float r = math::Vector3::Distance(p[i], p[i + 1]);				// Distance to next joint
						float lambda = len[i] / r;
						p[i+1] = math::lerp(p[i], p[i + 1], lambda);					// Next iter. position

						solve_constraint_forward_iter(i, p + i, orient + i, len[i]);	// Constrain iteration
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
				uint32_t num_float_alloc = m_num_link * (3 + 1 + 16) + 16 * 1;	// Trans (n), len (n), rot mat (n+1)
				void * alloc = ThomasCore::Core().Memory()->stack(0).allocate(sizeof(float) * num_float_alloc, sizeof(float));
				float *len = reinterpret_cast<float*>(alloc);
				math::Vector3 *p = reinterpret_cast<math::Vector3*>(len + m_num_link);
				math::Matrix *orientPtr = reinterpret_cast<math::Matrix*>(p + m_num_link);	// Ptr including parent orient used by first constraint
				math::Matrix *orient = orientPtr + 1;										// Updated bone orients

				// Find root parent orient
				if (m_rootParentBoneIndex < 0)
					*orientPtr = math::Matrix::Identity;
				else
					*orientPtr = math::extractRotation(objectPose[m_rootParentBoneIndex]);

				// Calc. distance of each bone/link
				uint32_t* chain = m_chain.get();
				float linkLengthSum = 0.f;
				uint32_t i = 0;
				for (; i < m_num_link - 1; i++) {												// Find position of each bone
					p[i] = objectPose[chain[i]].Translation();									// Bone joint position (initial)
					orient[i] = math::extractRotation(objectPose[chain[i]]);					// Get matrix orientation
					len[i] = math::Vector3::Distance(		
						p[i], objectPose[chain[i+1]].Translation());							// Bone length
					linkLengthSum += len[i];													// Sum chain length
				}
				m_originalTransform = objectPose[chain[i]];
				math::Quaternion targetO = math::Quaternion::CreateFromRotationMatrix(			// Calc. target orientation
					math::extractRotation(objectPose[chain[i]]));
				targetO = math::Quaternion::Slerp(targetO, m_targetOrient, m_orientationWeight);// End cases
				p[i] = objectPose[chain[i]].Translation();								
				math::Vector3 targetP = math::Vector3::Lerp(p[i], m_target, m_weight);			// Calc. target point
				orient[i] = math::Matrix::CreateFromQuaternion(targetO);						// --
				// Distance from root to target
				float targetDist = math::Vector3::Distance(
					objectPose[chain[0]].Translation(), targetP);
				/*if (targetDist > linkLengthSum)
					FABRIK_unreachable(targetP, len, p, m_num_link);
				else*/
					FABRIK_iteration(targetP, len, p, orient, m_num_link);
				math::Vector3 trans;
				math::Matrix pose;
				// Apply solution to chain
				for (i = 0; i < m_num_link - 1; i++) {
					pose = objectPose[chain[i]];
					trans = pose.Translation();
					pose.Translation(math::Vector3::Zero);										// Remove translation
					pose = pose * math::getMatrixRotationTo(pose.Up(), p[i + 1] - p[i]);		// Rotate bone Y toward child's point
					//pose = pose * skel.getBone(chain[i + 1])._invParentOrient;				// Apply orientation offset in relation to child (Y axis of the bone may not face child)
					pose.Translation(p[i]);														// Apply new translation
					objectPose[chain[i]] = pose;												// Set
				}
				// End case, apply target orient
				math::Vector3 up = math::Vector3::Transform(math::Vector3::Up, targetO);
				math::Vector3 right = math::Vector3::Transform(math::Vector3::Right, targetO);
				pose = objectPose[chain[m_num_link - 1]];
				trans = pose.Translation();
				pose.Translation(math::Vector3::Zero);											// Remove translation
				pose = pose * math::getMatrixRotationTo(pose.Up(), up);							// Rotate transform to y
				pose = pose * math::getMatrixRotationTo(pose.Right(), right);					// Rotate transform to x
				pose.Translation(p[m_num_link - 1]);											// Apply new translation
				objectPose[chain[m_num_link - 1]] = pose;

				const float GIZMO_LEN = 0.05f;
				const float MAX_JOINT_RAD = GIZMO_LEN * 2.5f;
				const float MAX_JOINT_LEN = GIZMO_LEN * 2.5f;
#ifdef _EDITOR
#ifdef IK_DRAW
				for (i = 0; i < m_num_link; i++) {
					float len = MAX_JOINT_LEN;
					// Calc. joint bounds
					JointInfo& joint = m_joint.get()[i];
					if (i < m_num_link - 1 && joint.limit_bend > math::EPSILON)
					{
						math::Matrix j = joint.orientation * orientPtr[i];
						float tan_a = std::tanf(joint.limit_bend);
						len = MAX_JOINT_RAD / tan_a;
						float rad = MAX_JOINT_LEN * tan_a;
						if (len > rad)
							len = MAX_JOINT_LEN;
						else
							rad = MAX_JOINT_RAD;
						math::Vector3 up = j.Up();
						up.Normalize();
						// Draw joint
						editor::Gizmos::Gizmo().SetColor(math::Color(0, 0, 1.f));
						editor::Gizmos::Gizmo().DrawRing(objectPose[chain[i]].Translation() + up * len, j.Up(), rad);
						// Calc. bone matrix axis length
						len = rad / std::sinf(joint.limit_bend);
					}
					// Draw bone matrix
					editor::Gizmos::Gizmo().DrawMatrixBasis(objectPose[chain[i]], math::Vector3(GIZMO_LEN, len, GIZMO_LEN));
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
				int validCount = m_chain.get()[0] < skel.getNumBones();;
				for (uint32_t i = 1; i < m_num_link; i++) {
					math::Vector3 vn = skel.getBone(i)._bindPose.Translation();
					sum += math::Vector3::Distance(v, vn);
					v = vn;
					validCount += m_chain.get()[i] < skel.getNumBones();
				}
				m_chainLength = sum;
				if (validCount != m_num_link)
					return false;
				// Extract extra parameters
				m_rootParentBoneIndex = skel.getBone(m_chain.get()[0])._parentIndex;
				for (uint32_t i = 0; i < m_num_link; i++)
				{
					// Extract rotation to the base
					m_joint.get()[i].base_offset = math::extractRotation(skel.getBone(m_chain.get()[i])._bindPose);
					m_joint.get()[i].refreshOrient();
				}
				return true;
			}
			float IK_FABRIK_C_Constraint::getChainLength()
			{
				return m_chainLength;
			}
			uint32_t IK_FABRIK_C_Constraint::getRootBoneIndex()
			{
				return m_chain.get()[0];
			}
			void IK_FABRIK_C_Constraint::JointInfo::refreshOrient()
			{
				this->orientation = orient_offset * base_offset;
				this->backwardOrient = orientation.Invert();
			}
}
	}
}