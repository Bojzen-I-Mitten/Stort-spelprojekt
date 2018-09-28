#pragma once

#include <stdint.h>
#include "data/TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			
			class WeightMixer {

			private:

				WeightTripple *m_weights;
				uint32_t m_numWeight;

			public:
				WeightMixer(){}
				virtual ~WeightMixer() {}

				virtual void update(float dT) = 0;

			public:
				const WeightTripple* getWeights();
				uint32_t numWeights();
			};
		}
	}
}