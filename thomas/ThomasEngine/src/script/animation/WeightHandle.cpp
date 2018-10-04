#include "WeightHandle.h"



namespace ThomasEngine
{
	namespace Script
	{


		WeightHandle::WeightHandle(unsigned int numChannel)
			: m_mixer(new thomas::graphics::animation::WeightMixer(numChannel, numChannel))
		{
			m_mixer->setSingleWeightMode();
			this->m_WeightData = m_mixer->m_weights;

			
		}

		WeightHandle::~WeightHandle()
		{
		}

		void WeightHandle::setWeight(unsigned int index, WeightTripple weight)
		{
			m_mixer->m_weights[index].m_scale = weight.m_scale;
			m_mixer->m_weights[index].m_rot = weight.m_rot;
			m_mixer->m_weights[index].m_translation = weight.m_translation;
		}

		thomas::graphics::animation::WeightMixer * WeightHandle::Mixer()
		{
			return m_mixer;
		}

		thomas::graphics::animation::WeightTripple WeightTripple::convert()
		{
			return thomas::graphics::animation::WeightTripple(m_scale, m_rot, m_translation);
		}

		WeightTripple::WeightTripple(float weight)
			: m_scale(weight), m_rot(weight), m_translation(weight)
		{}
		WeightTripple::WeightTripple(float scaleWeight, float rotationWeight, float translationWeight)
			: m_scale(scaleWeight), m_rot(rotationWeight), m_translation(translationWeight)
		{}
		WeightTripple WeightTripple::fromWeight(float weight)
		{
			WeightTripple w;
			w.m_scale = weight;
			w.m_rot = weight;
			w.m_translation = weight;
			return w;
		}
		WeightTripple WeightTripple::fromWeight(float scale, float rot, float trans)
		{
			WeightTripple w;
			w.m_scale = scale;
			w.m_rot = rot;
			w.m_translation = trans;
			return w;
		}
	}
}