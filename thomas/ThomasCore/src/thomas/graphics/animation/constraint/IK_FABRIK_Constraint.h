#pragma once
#include <vector>
#include <memory>
#include "BoneConstraint.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			constexpr float FABRIK_TOLERANCE = 1.e-5f;
			constexpr float MAX_FABRIK_ITER = 60;

			/* Transform 'input' constraint, bone position is specified externally through a transform component.
			*/
			class IK_FABRIK_Constraint
				: public BoneConstraint
			{
			public:

				struct LinkParameter 
				{
					uint32_t m_index;
				};

			public:


				IK_FABRIK_Constraint(uint32_t num_link);
				IK_FABRIK_Constraint(const std::vector<LinkParameter>& link_chain);
				~IK_FABRIK_Constraint();

				void setLinkIndex(uint32_t chainIndex, uint32_t boneIndex);

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;

				math::Vector3 m_target;					// Target in object space.

			private:

				std::unique_ptr<LinkParameter> m_chain;	// Link chain paramters (root at 0)
				uint32_t m_num_link;					// Number of links
			};

		}
	}
}