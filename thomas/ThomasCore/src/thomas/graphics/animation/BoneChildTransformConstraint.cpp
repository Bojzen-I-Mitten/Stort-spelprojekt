#include "BoneChildTransformConstraint.h"
#include "data/Skeleton.h"

namespace thomas {
	namespace graphics {
		namespace animation {



			BoneChildTransformConstraint::BoneChildTransformConstraint(object::component::Transform & reference)
				: m_ref(&reference)
			{
			}

			BoneChildTransformConstraint::~BoneChildTransformConstraint()
			{
			}

			void BoneChildTransformConstraint::execute(Skeleton& skel, math::Matrix* objectPose, uint32_t boneInd)
			{
				// If (transform is childTransform)
				objectPose[boneInd] = math::Matrix::CreateScale(0.01f) * m_ref->GetLocalWorldMatrix();

				// If (Transform is World)
				//objectPose[boneInd] = m_ref->getWorldMatrix() * /* object world inverse... */ * objectPose[skel.getBone(boneInd)._parentIndex].Invert();
			}

		}
	}
}