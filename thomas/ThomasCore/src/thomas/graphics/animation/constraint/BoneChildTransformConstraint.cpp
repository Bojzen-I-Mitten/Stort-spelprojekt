#include "BoneChildTransformConstraint.h"
#include "../data/Skeleton.h"
#include "../../../object/component/Transform.h"

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

			void BoneChildTransformConstraint::execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd)
			{
				math::Vector3 s = math::extractAxisScale(objectPose[boneInd]);
				// If (transform is childTransform)
				objectPose[boneInd] = math::mult(m_ref->GetLocalWorldMatrix(), s);

				// If (Transform is World)
				//objectPose[boneInd] = m_ref->getWorldMatrix() * /* object world inverse... */ * objectPose[skel.getBone(boneInd)._parentIndex].Invert();
			}

			bool BoneChildTransformConstraint::apply(Skeleton & skel, uint32_t boneInd)
			{
				return true;
			}

		}
	}
}