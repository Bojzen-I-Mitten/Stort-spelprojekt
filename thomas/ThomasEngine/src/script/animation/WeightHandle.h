#pragma once

#pragma unmanaged

#include <thomas/graphics/animation/WeightMixer.h>

#pragma managed

namespace ThomasEngine
{
	namespace Script
	{

		ref struct WeightTripple {
			float m_scale, m_rot, m_translation;
		};
		enum class Mode 
		{
			PerNode = 0,		// Weighted per node
			PerChannel = 1,		// Weighted per node channel
			Additive = 2
		};

		ref class WeightHandle {
		public:
			/* Generate  */
			WeightHandle(unsigned int numChannel);
			~WeightHandle();

			thomas::graphics::animation::WeightTripple* m_WeightData;

			void setWeight(unsigned int index, WeightTripple weight);

			thomas::graphics::animation::WeightMixer * Mixer();

		private:

			thomas::graphics::animation::WeightMixer * m_mixer;
		};

	}
}