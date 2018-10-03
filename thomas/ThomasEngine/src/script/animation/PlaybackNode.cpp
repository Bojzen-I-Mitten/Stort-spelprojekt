#include "PlaybackNode.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/data/AnimationData.h>
#include <thomas/graphics/animation/data/Skeleton.h>
#include <thomas/graphics/animation/BaseAnimationTime.h>

#pragma managed
#include "../../resource/Model.h"
#include "../../resource/Animation.h"

namespace ThomasEngine
{
	namespace Script
	{
		PlaybackNode::PlaybackNode(Model ^ model, Animation^ anim)
		 :	PlaybackNode(model, anim, false)
		{		}
		PlaybackNode::PlaybackNode(Model ^ model, Animation^ anim, bool loop)
		{
			using namespace thomas::graphics::animation;


			thomas::resource::Model *m = (thomas::resource::Model*)model->m_nativePtr;
			thomas::graphics::animation::Skeleton *skel = m->GetSkeleton();
			if (!skel)
				throw gcnew System::NotSupportedException(
					"Playback node can't be created: Skeleton does not exist in model " +
					Utility::ConvertString(m->GetName()));

			AnimationData * data = anim->Native()->GetAnimation();

			std::unique_ptr<Playback> playback(new BaseAnimationTime(0.f, data->m_duration, loop ? PlayType::Loop : PlayType::Once));
			m_node = new AnimPlayback(*skel, playback, *data);
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