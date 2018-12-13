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

				/* Reset playback, state is set to a new play instance. */
				void playOnce();
				/* Reset playback time. Does not modify play state and does not ensure playback is played.*/
				void resetTime();

			public:
				PlayType getPlayType();
				void setPlayType(PlayType type);
				float getSpeed();
				void setSpeed(float speed);
				float getDuration();
				/* Check if animation is running (Looping or PlayOnce + time left) */
				bool isRunning();


			private:
				PlayType m_playType;
				float m_duration;
				float m_speedUp;
			};
		}
	}
}