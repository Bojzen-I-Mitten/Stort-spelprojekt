#pragma once

#include <stdint.h>
#include "data/TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			
			class WeightMixer {
			public:
				enum Mode 
				{
					PerNode,		// Weighted per node
					PerChannel		// Weighted per node channel
				};
				WeightTripple *m_weights;
				Mode * m_mode;
			public:

				WeightMixer(uint32_t numWeight, uint32_t numChannel);
				virtual ~WeightMixer();

				virtual void update(float dT) = 0;

			public:
				virtual const WeightTripple* getWeights();
				virtual const Mode* getMode();
			protected:
				void clearWeights(uint32_t at, uint32_t num);
			};


			class WeightLinearPerChannel : public WeightMixer {
			private:
				float m_duration, m_elapsed;
				uint32_t m_index, m_weightedNodes;
			public:

				WeightLinearPerChannel(uint32_t NumweightedNodes, float timePerNode);
				virtual void update(float dT) override;
			};
		}
	}
}