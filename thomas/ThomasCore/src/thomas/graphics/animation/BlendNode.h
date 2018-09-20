#pragma once
#include "AnimationNode.h"
#include "Playback.h"
#include <memory>

namespace thomas {
	namespace graphics {
		namespace animation {

			class BlendNode : public AnimationNode
			{
			public:
				BlendNode(Skeleton &ref);
				virtual ~BlendNode();



			private:

				std::unique_ptr<Playback> time;
			};
		}
	}
}