#include "AnimationNode.h"


namespace thomas {
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
			mat = math::Matrix::CreateScale(calcBoneScale(bone));
			mat = math::Matrix::CreateFromQuaternion(calcBoneRot(bone)) * mat;
			mat.Translation(calcBonePosition(bone));
			return mat;

		}

	}
}