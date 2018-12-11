#pragma once
#include "../AnimationNode.h"
#include "../WeightMixer.h"
#include <memory>

namespace thomas {
	namespace graphics {
		namespace animation {

			class BarycentricBlendNode : public AnimationNode
			{
			public:
				BarycentricBlendNode(Skeleton &ref);
				virtual ~BarycentricBlendNode();
				
			private:

				std::unique_ptr<WeightMixer> m_weights;
			};
		}
	}
}
