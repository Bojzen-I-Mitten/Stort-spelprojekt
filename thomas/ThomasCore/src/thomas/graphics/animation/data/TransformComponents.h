#pragma once

#include "../../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			/* Per channel weights
			*/
			struct WeightTripple {
				// Weight values
				float m_scale, m_rot, m_translation;

				WeightTripple() {}
				WeightTripple(float weight)
					: m_scale(weight), m_rot(weight), m_translation(weight)
				{}
				WeightTripple(float scaleWeight, float rotationWeight, float translationWeight)
					: m_scale(scaleWeight), m_rot(rotationWeight), m_translation(translationWeight)
				{}
			};
			/* Transformation components. 
			*/
			struct TransformComponents {
				math::Vector3 m_scale;
				math::Quaternion m_rot;
				math::Vector3 m_pos;

				TransformComponents(){}
				TransformComponents(math::Matrix &m) {
					Decompose(m);
				}
				/* Blend the transform components from the current state to the target, with the weight.
				* target	<<	Target transform to blend toward
				* weight	<<	Influence weight in interval [0,1]
				*/
				void blendTo(TransformComponents & target, float weight) {

					m_scale += target.m_scale * weight;
					m_rot = math::Quaternion::Slerp(m_rot, target.m_rot, weight);
					m_pos += target.m_pos * weight;
				}
				/* Blend the transform components from the current state to the target, with the weight.
				* target	<<	Target transform to blend toward
				* weight	<<	Influence weights for each channel (3) in interval [0,1]
				*/
				void blendTo(TransformComponents & target, const WeightTripple& weight) {

					m_scale += target.m_scale * weight.m_scale;
					m_rot = math::Quaternion::Slerp(m_rot, target.m_rot, weight.m_rot);
					m_pos += target.m_pos * weight.m_translation;
				}

				/* Construct a component by decomposing the matrix
				*/
				void Decompose(math::Matrix &m) {
					m.Decompose(m_scale, m_rot, m_pos);
				}
			};
		}
	}
}