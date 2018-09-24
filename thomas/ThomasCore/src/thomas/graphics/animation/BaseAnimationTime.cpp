#include "BaseAnimationTime.h"
#include <algorithm>

namespace thomas {
	namespace graphics {
		namespace animation {
			BaseAnimationTime::BaseAnimationTime(float startAt, float duration, PlayType playType)
				: Playback(startAt), m_duration(duration), m_speedUp(1.f), m_playType(playType)
			{
			}
			BaseAnimationTime::~BaseAnimationTime()
			{
			}
			void BaseAnimationTime::timeStep(float dT)
			{
				m_elapsed += dT * m_speedUp;
				if (m_duration < m_elapsed) {
					switch (m_playType)
					{
					case PlayType::Loop:
						m_elapsed = std::fmodf(m_elapsed, m_duration);	// Loop time
						break;
					case PlayType::Once:
					default:
						m_elapsed = m_duration;
						break;
					}
				}
			}
		}
	}
}