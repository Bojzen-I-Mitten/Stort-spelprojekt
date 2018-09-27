#pragma once

#include <stdint.h>

namespace thomas {
	namespace graphics {
		namespace animation {

			class WeightMixer {

			private:

				float *m_weights;
				uint32_t m_numWeight;

			public:
				const float* getWeights();
				uint32_t numWeights();

				WeightMixer(){}
				virtual ~WeightMixer() {}
			};
		}
	}
}