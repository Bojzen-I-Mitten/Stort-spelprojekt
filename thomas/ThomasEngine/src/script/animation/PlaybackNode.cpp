#include "PlaybackNode.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/data/AnimationData.h>
#include <thomas/graphics/animation/data/Skeleton.h>
#include <thomas/graphics/animation/BaseAnimationTime.h>

#pragma managed

namespace ThomasEngine
{
	namespace Script
	{
		PlaybackNode::PlaybackNode(Animation^ anim)
		{
			using namespace thomas::graphics::animation;

			AnimationData * data = anim->Native()->GetAnimation();

			std::unique_ptr<Playback> playback(new BaseAnimationTime(0.f, data->m_duration, PlayType::Once));
			AnimationNode *node = new AnimPlayback(m_node->m_ref, playback, *data);
		}
		PlaybackNode::~PlaybackNode()
		{
		}
		thomas::graphics::animation::AnimationNode * PlaybackNode::Native()
		{
			return m_node;
		}
	}
}