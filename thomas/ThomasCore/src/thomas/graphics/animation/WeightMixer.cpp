#include "WeightMixer.h"



namespace thomas {
	namespace graphics {
		namespace animation {



			const WeightTripple * WeightMixer::getWeights()
			{
				return m_weights;
			}

			uint32_t WeightMixer::numWeights()
			{
				return m_numWeight;
			}

		}
	}
}