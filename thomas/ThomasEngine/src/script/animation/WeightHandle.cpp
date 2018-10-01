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

	}
}