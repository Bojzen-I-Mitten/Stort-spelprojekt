#pragma once



namespace thomas {
	namespace animation {

		class Playback
		{
		public:
			Playback();
			~Playback();

			virtual float update(float dT);

		private:

			float _elapTime;
			float _duration;
		};

	}
}

