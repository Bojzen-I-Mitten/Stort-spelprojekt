#pragma once
#include <vector>
#include <memory>
#include "BoneConstraint.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			constexpr float FABRIK_TOLERANCE = e-5;
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


				IK_FABRIK_Constraint(const std::vector<LinkParameter>& link_chain);
				~IK_FABRIK_Constraint();

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, uint32_t boneInd) override;

			private:

				std::unique_ptr<LinkParameter> m_chain;	// Link chain paramters (root at 0)
				uint32_t m_num_link;					// Number of links
				math::Vector3 m_target;					// Target in object space.
			};

		}
	}
}