#pragma once
#include <vector>
#include <memory>
#include "BoneConstraint.h"
#include "IK_FABRIK_Constraint.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			
			/* Transform 'input' constraint, bone position is specified externally through a transform component.
			*/
			class IK_FABRIK_C_Constraint
				: public BoneConstraint
			{
			public:
				struct JointParams
				{
					math::Matrix orientation = math::Matrix::Identity;		// Boundary orientation offset (rotation)
					float limit_bend = math::PI / 2 - 0.01f * math::PI;
					float limit_twist = math::PI / 4;
				};

				struct LinkParameter 
				{
					uint32_t m_index;
					JointParams m_jointInfo;

					LinkParameter(){}
					LinkParameter(uint32_t index) : m_index(index), m_jointInfo() {}
					LinkParameter(uint32_t index, JointParams info) : m_index(index), m_jointInfo(info) {}
				};
			private:
				struct JointInfo
				{
					math::Matrix orientation = math::Matrix::Identity;		// Joint orientation offset in relation to parent (rotation from parent orienation to joint orient)
					float limit_bend = math::PI / 2 - 0.01f * math::PI;
					float limit_twist = math::PI / 4;


					math::Matrix base_offset = math::Matrix::Identity;		// Rotation offset from parent orientation
					math::Matrix orient_offset = math::Matrix::Identity;		// Rotation offset specified
				};
			private:
				void FABRIK_unreachable(math::Vector3 target, float *d, math::Vector3*p, uint32_t num_link);
				void solve_constraint_backward_iter(math::Vector3 *p_c, math::Matrix *c_orient, float bone_len);
				void solve_constraint_forward_iter(uint32_t index, math::Vector3 *p_c, math::Matrix *c_orient, float bone_len);
				void FABRIK_iteration(math::Vector3 target, float *len, math::Vector3*p, math::Matrix *orient, uint32_t num_link);

			public:


				IK_FABRIK_C_Constraint(uint32_t num_link);
				IK_FABRIK_C_Constraint(const std::vector<LinkParameter>& link_chain);
				~IK_FABRIK_C_Constraint();
				/* Specify information for a bone link
				*/
				void setLinkAtIndex(uint32_t chainIndex, LinkParameter param);
				/* Specify joint information for a bone
				*/
				void setJointAtIndex(uint32_t chainIndex, JointParams info);

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;
				virtual bool apply(Skeleton& skel, uint32_t boneInd) override;

				math::Vector3 m_target;					// Target in object space.
				math::Quaternion m_targetOrient;		// Orientation at target
				float m_weight;							// IK weight for target point
				float m_orientationWeight;				// IK weight for orientation

				float getChainLength();					// Access bone chain length
				uint32_t getSrcBoneIndex();				// Get bone index of bone chain root


			private:

				std::unique_ptr<uint32_t> m_chain;		// Link chain bone indices (root at 0)
				std::unique_ptr<JointInfo> m_joint;		// Link joint parameters (root at 0)
				uint32_t m_num_link;					// Number of links
				int m_rootParentBoneIndex;				// Chain root parent bone index (-1 if it does not exist)
				float m_chainLength;					// Updated length of bone chain (originates as bind pose chain length)
			};

		}
	}
}