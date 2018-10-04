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
					PerNode = 0,		// Weighted per node
					PerChannel = 1,		// Weighted per node channel
					Additive = 2
				};
				WeightTripple *m_weights;
				Mode * m_mode;
				uint32_t m_numChannel;
			public:

				WeightMixer(uint32_t numWeight, uint32_t numChannel);
				virtual ~WeightMixer();

				virtual void update(float dT) {};


			public:
				const WeightTripple* getWeights();
				const Mode* getMode();

				void setSingleWeightMode();
				void setMode(Mode m);
				
			protected:
				void clearWeights(uint32_t at, uint32_t num);
			};


			class WeightLinearPerChannel : public WeightMixer {
			private:
				float m_duration, m_elapsed;
				uint32_t m_index;
			public:

				WeightLinearPerChannel(uint32_t NumweightedNodes, float timePerNode);
				virtual void update(float dT) override;
			};
		}
	}
}