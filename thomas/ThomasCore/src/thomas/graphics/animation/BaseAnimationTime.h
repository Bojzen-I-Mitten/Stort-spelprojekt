#pragma once
#include "Playback.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			enum PlayType
			{
				None = 0,
				Once = 1,
				Loop = 2
			};

			class BaseAnimationTime : public Playback
			{
			public:
				BaseAnimationTime(float startAt, float duration, PlayType playType);
				virtual ~BaseAnimationTime();


				virtual void timeStep(float dT);	// Step elapsed time
				virtual float getTime();			// Get the playback time

			private:
				float m_duration;
				float m_elapsed;
				float m_speedUp;
				PlayType m_playType;
			};
		}
	}
}