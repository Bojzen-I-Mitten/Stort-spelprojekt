#pragma once



namespace thomas {
	namespace animation {

		class Playback
		{
		public:
			Playback();
			~Playback();

			virtual void timeStep(float dT) = 0;	// Step elapsed time
			virtual float getTime() = 0;			// Get the playback time

		private:

			float m_elapTime;
			float m_duration;
		};

	}
}

