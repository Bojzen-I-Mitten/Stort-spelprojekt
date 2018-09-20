#include "AnimationNode.h"
#include "data/Skeleton.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimationNode::AnimationNode(Skeleton &ref)
				: m_ref(ref), m_boneMapping(ref.getNumBones())
			{
				for (size_t i = 0; i < m_boneMapping.size(); i++)
					m_boneMapping[i] = -1;
			}


			AnimationNode::~AnimationNode()
			{
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
		}
	}
}