#pragma once

#pragma unmanaged

#include <thomas/graphics/animation/WeightMixer.h>

#pragma managed

namespace ThomasEngine
{
	namespace Script
	{

		/* Contains weights for each transform component (scale, rot, translation)
		*/
		public value struct WeightTripple {
			float m_scale, m_rot, m_translation;

			WeightTripple(float weight);
			WeightTripple(float scale, float rot, float trans);
			/* Sum of absolute values
			*/
			float AbsoluteSum();
			float Average();

			static WeightTripple fromWeight(float weight);
			static WeightTripple fromWeight(float scale, float rot, float trans);
			static WeightTripple fromWeight(thomas::graphics::animation::WeightTripple w);

			thomas::graphics::animation::WeightTripple convert();
		};

		public enum class Mode
		{
			PerNode = 0,		// Weighted per node
			PerChannel = 1,		// Weighted per node channel
			Additive = 2
		};

		public ref class WeightHandle {
		public:
			/* Generate  */
			WeightHandle(unsigned int numChannel);
			~WeightHandle();

			thomas::graphics::animation::WeightTripple* m_WeightData;

			/*	Set weights at the bone at index
			*/
			void setWeight(unsigned int index, WeightTripple weight);
			void setWeight(unsigned int index, float weight);
			void setWeight(unsigned int index, float scale_w, float rot_w, float trans_w);

			WeightTripple getWeight(unsigned int index);

			thomas::graphics::animation::WeightMixer * Mixer();

		private:

			thomas::graphics::animation::WeightMixer * m_mixer;
		};

	}
}