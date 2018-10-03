#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/AnimPlayback.h>

#pragma managed


namespace ThomasEngine
{
	ref class Model;
	ref class Animation;

	namespace Script
	{
		/* Wrapper for AnimationNode, referencing a single animation applicable in a blend tree.
		*/
		public ref class PlaybackNode
		{
		public:

			PlaybackNode(Model ^ model, Animation^ anim);
			PlaybackNode(Model ^ model, Animation ^ anim, bool loop);
			~PlaybackNode();


			thomas::graphics::animation::AnimationNode* Native();
		private:

			thomas::graphics::animation::AnimPlayback* m_node;
		};

	}
}