#pragma once
#include "BoneConstraint.h"

namespace thomas {
	namespace object {
		namespace component {
			class Transform;
		}
	}
	namespace graphics {
		namespace animation {

			/* Transform 'input' constraint, bone position is specified externally through a transform component.
			*/
			class BoneChildTransformConstraint 
				: public BoneConstraint
			{
			public:
				BoneChildTransformConstraint(object::component::Transform & m_ref);
				~BoneChildTransformConstraint();

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;
				virtual bool apply(Skeleton& skel, uint32_t boneInd) override;

			private:
				object::component::Transform * m_ref;
			};

		}
	}
}