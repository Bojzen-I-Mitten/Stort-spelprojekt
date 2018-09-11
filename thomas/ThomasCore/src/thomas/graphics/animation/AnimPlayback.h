#pragma once
#include "Playback.h"

namespace thomas {
	namespace animation {

		class AnimPlayback :	public Playback
		{
		public:
			AnimPlayback();
			virtual ~AnimPlayback();


			virtual float update(float dT) = 0;

		};

	}
}

