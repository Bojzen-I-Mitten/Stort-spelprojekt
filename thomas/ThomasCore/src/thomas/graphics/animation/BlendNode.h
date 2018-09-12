#pragma once
#include "AnimationNode.h"
#include "Playback.h"
#include <memory>

namespace thomas {
	namespace animation {

		class BlendNode :	public AnimationNode
		{
		public:
			BlendNode();
			virtual ~BlendNode();



		private:

			std::unique_ptr<Playback> time;
		};

	}
}
