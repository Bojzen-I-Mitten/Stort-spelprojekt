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

				void setPlayType(PlayType type);
				PlayType getPlayType();
				void setSpeed(float speed);

				/* Partial state reset, state is set to a new play instance. */
				void playOnce();

			private:
				PlayType m_playType;
				float m_duration;
				float m_speedUp;
			};
		}
	}
}