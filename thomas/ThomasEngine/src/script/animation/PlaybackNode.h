#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/AnimPlayback.h>

#pragma managed
#include "../../resource/Animation.h"


namespace ThomasEngine
{
	namespace Script
	{
		/* Wrapper for AnimationNode, referencing a single animation applicable in a blend tree.
		*/
		public ref class PlaybackNode
		{
		public:

			PlaybackNode(Animation^ anim);
			~PlaybackNode();


			thomas::graphics::animation::AnimationNode* Native();
		private:

			thomas::graphics::animation::AnimPlayback* m_node;
		};

	}
}