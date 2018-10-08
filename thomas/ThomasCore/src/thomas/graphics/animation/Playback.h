#pragma once



namespace thomas {
	namespace graphics {
		namespace animation {

			class Playback
			{
			public:
				Playback();
				Playback(float startAt) : m_elapsedTime(startAt){}
				virtual ~Playback();

				virtual void timeStep(float dT) {};	// Step elapsed time

			public:
				/* Elapsed time. */
				float m_elapsedTime;
			};
		}
	}
}

