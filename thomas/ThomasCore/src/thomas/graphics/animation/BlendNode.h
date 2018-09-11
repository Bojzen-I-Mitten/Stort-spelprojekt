#pragma once
#include "AnimationNode.h"
#include "Playback.h"

namespace thomas {
	namespace animation {

		class BlendNode :	public Playback
		{
		public:
			BlendNode();
			virtual ~BlendNode();
		};

	}
}
