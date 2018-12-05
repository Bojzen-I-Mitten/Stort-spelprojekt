#pragma once
#include <vector>
#include <memory>
#include "BoneConstraint.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			constexpr float FABRIK_TOLERANCE = 1.e-5f;
			constexpr float MAX_FABRIK_ITER = 60;

			math::Matrix weightRotationBetween(math::Vector3 from, math::Vector3 dest, float weight);

			/* Transform 'input' constraint, bone position is specified externally through a transform component.
			*/
			class IK_FABRIK_Constraint
				: public BoneConstraint
			{
			public:

				struct LinkParameter 
				{
					uint32_t m_index;

					LinkParameter(){}
					LinkParameter(uint32_t index) : m_index(index) {}
				};

			public:


				IK_FABRIK_Constraint(uint32_t num_link);
				IK_FABRIK_Constraint(const std::vector<LinkParameter>& link_chain);
				~IK_FABRIK_Constraint();

				void setLinkAtIndex(uint32_t chainIndex, LinkParameter boneIndex);

				// Replaces the bone transform from a separate object.
				virtual void execute(Skeleton& skel, math::Matrix* objectPose, TransformComponents* comp, uint32_t boneInd) override;
				virtual bool apply(Skeleton& skel, uint32_t boneInd) override;

				math::Vector3 m_target;					// Target in object space.
				math::Quaternion m_targetOrient;		// Orientation at target
				float m_weight;							// IK weight

			private:

				std::unique_ptr<LinkParameter> m_chain;	// Link chain paramters (root at 0)
				uint32_t m_num_link;					// Number of links
			};

		}
	}
}