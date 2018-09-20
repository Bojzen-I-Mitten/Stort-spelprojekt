#pragma once



namespace thomas {
	namespace graphics {
		namespace animation {

			class Playback
			{
			public:
				Playback();
				virtual ~Playback();

				virtual void timeStep(float dT) = 0;	// Step elapsed time
				virtual float getTime()			= 0;	// Get the playback time

			private:

				float m_elapTime;
				float m_duration;
			};
		}
	}
}

