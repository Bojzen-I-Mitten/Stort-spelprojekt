#include "WeightMixer.h"



namespace thomas {
	namespace graphics {
		namespace animation {


			
			WeightMixer::WeightMixer(uint32_t numWeight, uint32_t numChannel)
				: m_weights(new WeightTripple[numWeight]), m_mode(new Mode[numChannel]), m_numChannel(numChannel)
			{
			}

			WeightMixer::~WeightMixer()
			{
				delete[] m_weights;
				delete[] m_mode;
			}

			const WeightTripple * WeightMixer::getWeights()
			{
				return m_weights;
			}

			const WeightMixer::Mode * WeightMixer::getMode()
			{
				return m_mode;
			}

			void WeightMixer::setSingleWeightMode()
			{
				std::memset(m_mode, 0, sizeof(Mode) * m_numChannel);
			}

			void WeightMixer::setMode(Mode m)
			{
				Mode *ptr = m_mode;
				while (++ptr != m_mode + m_numChannel)
					*ptr = m;
			}

			void WeightMixer::clearWeights(uint32_t at, uint32_t num)
			{
				// Need ifdef ~ SYS_SUPPORT_FLOAT_MEMSET
				std::memset(m_weights + at, 0, num * sizeof(WeightTripple));
			}



					   
			WeightLinearPerChannel::WeightLinearPerChannel(uint32_t NumweightedNodes, float timePerNode)
				: WeightMixer(NumweightedNodes, NumweightedNodes), m_duration(timePerNode), m_elapsed(0.f), m_index(0)
			{
				setSingleWeightMode();
			}

			void WeightLinearPerChannel::update(float dT)
			{
				if (m_index == m_numChannel) return;

				// Step index
				m_elapsed += dT;
				float norm = m_elapsed / m_duration;
				int floor = int(norm);
				if (floor) {
					clearWeights(m_index, floor);
					m_index += floor;
					float cycles = float(floor);
					norm -= float(floor);
					m_elapsed = norm * m_duration;
					// If last blend target
					if (m_index >= m_numChannel) {
						m_index = m_numChannel; // In case of overstep
						m_weights[m_numChannel - 1] = WeightTripple(1.f);
						return;
					}
				}

				// Update weights
				m_weights[m_index] = WeightTripple(1.f - norm);
				m_weights[m_index + 1] = WeightTripple(norm);
			}

		}
	}
}