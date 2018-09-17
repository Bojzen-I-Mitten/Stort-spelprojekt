#include "AnimationNode.h"


namespace thomas {
	namespace graphics {
		namespace animation {

			AnimationNode::AnimationNode()
			{
			}


			AnimationNode::~AnimationNode()
			{
			}

			math::Matrix AnimationNode::calcLocalTransform(unsigned int bone)
			{
				math::Matrix mat;										// Interpolated transform
				mat = math::Matrix::CreateScale(calcBoneScale(bone)) * math::Matrix::CreateFromQuaternion(calcBoneRot(bone));
				math::Vector3 pos = calcBonePosition(bone);
				mat.Translation(pos);
				return mat;

			}
		}
	}
}