#include "AnimationNode.h"
#include "data/Skeleton.h"
#include "../../utils/Utility.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			AnimationNode::AnimationNode(Skeleton &ref)
				: AnimationNode(ref, ref.getNumBones())
			{
			}

			AnimationNode::AnimationNode(Skeleton & ref, uint32_t mappingReserve)
				: m_ref(ref), m_flag(0), m_boneMapping()
			{
				m_boneMapping.reserve(mappingReserve);	// Reserve to the number of bones
				/*
				// Set all bones to 'inactive' initially.
				for (size_t i = 0; i < m_boneMapping.size(); i++)
					m_boneMapping[i] = -1;
				*/
			}


			AnimationNode::~AnimationNode()
			{
			}
			uint32_t AnimationNode::NumChannel()
			{
				return (uint32_t)m_boneMapping.size();
			}

			math::Matrix AnimationNode::calcLocalTransform(unsigned int bone)
			{	
				if (m_boneMapping[bone] != -1) {
					math::Vector3 t, s;
					math::Quaternion r;
					calcFrame(bone, t, s, r);
					// Interpolated transform
					math::Matrix mat = math::Matrix::CreateScale(s);
					mat = mat * math::Matrix::CreateFromQuaternion(r);
					mat.Translation(t);
					return mat;
				}
				return m_ref.getBone(bone)._bindPose;
			}
			void AnimationNode::markUpdated()
			{
				utility::setFlag(m_flag, ANIM_UPDATED_FLAG);
			}
			uint32_t AnimationNode::isUpdated()
			{
				return utility::hasFlag(m_flag, ANIM_UPDATED_FLAG);
			}
			void AnimationNode::resetUpdate()
			{
				utility::rmvFlag(m_flag, ANIM_UPDATED_FLAG);
			}
			BindPoseNode::BindPoseNode(Skeleton & ref)
				: AnimationNode(ref)
			{
			}
			BindPoseNode::~BindPoseNode()
			{
			}
			void BindPoseNode::update(float)
			{
			}
			math::Vector3 BindPoseNode::calcBonePosition(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Vector3 BindPoseNode::calcBoneScale(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Quaternion BindPoseNode::calcBoneRot(unsigned int bone)
			{
				return math::Quaternion();
			}
			void BindPoseNode::calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot)
			{

			}
			void BindPoseNode::calcFrame(TransformComponents * result)
			{
				std::memcpy(result, m_ref.getBindComponents(), sizeof(TransformComponents) * m_ref.getNumBones());
			}
		}
	}
}