#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/AnimPlayback.h>
#include <thomas/graphics/animation/BaseAnimationTime.h>
#include <thomas/graphics/animation/data/Skeleton.h>

#pragma managed


namespace ThomasEngine
{
	ref class Model;
	ref class Animation;
	ref class PlaybackHandle;

	namespace Script
	{

		ref class PlaybackHandle;

		/* Wrapper for AnimationNode, referencing a single animation applicable in a blend tree.
		*/
		public ref class PlaybackNode
		{
		public:

			PlaybackNode(Model ^ model, Animation^ anim);
			PlaybackNode(Model ^ model, Animation ^ anim, bool loop);
			PlaybackNode(thomas::graphics::animation::Skeleton& model, Animation ^ anim, bool loop);
			void Initiate(thomas::graphics::animation::Skeleton& skel, Animation ^ anim, bool loop);
			~PlaybackNode();

			PlaybackHandle^ getTimeHandle();

			thomas::graphics::animation::AnimationNode* Native();
		private:

			thomas::graphics::animation::BaseAnimationTime * m_playController;
			thomas::graphics::animation::AnimPlayback* m_node;
			PlaybackHandle^ m_timeHandle;
		};

	}
}