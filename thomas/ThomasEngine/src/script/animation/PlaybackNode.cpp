#include "PlaybackNode.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/data/AnimationData.h>

#pragma managed
#include "../../resource/Model.h"
#include "../../resource/Animation.h"
#include "PlaybackHandle.h"

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
			if(!anim)
				throw gcnew System::NotSupportedException(
					"Playback node can't be created: Animation was null ");

			AnimationData * data = anim->Native()->GetAnimation();

			m_playController = new BaseAnimationTime(0.f, data->m_duration, loop ? PlayType::Loop : PlayType::Once);
			std::unique_ptr<Playback> playback(m_playController);
			m_node = new AnimPlayback(*skel, playback, *data);

			m_timeHandle = gcnew PlaybackHandle(m_playController);
		}
		PlaybackNode::PlaybackNode(thomas::graphics::animation::Skeleton& skel, Animation^ anim, bool loop)
		{
			Initiate(skel, anim, loop);
		}

		void PlaybackNode::Initiate(thomas::graphics::animation::Skeleton& skel, Animation^ anim, bool loop)
		{

			using namespace thomas::graphics::animation;
			if (!anim)
				throw gcnew System::NotSupportedException(
					"Playback node can't be created: Animation was null ");

			AnimationData * data = anim->Native()->GetAnimation();

			m_playController = new BaseAnimationTime(0.f, data->m_duration, loop ? PlayType::Loop : PlayType::Once);
			std::unique_ptr<Playback> playback(m_playController);
			m_node = new AnimPlayback(skel, playback, *data);

			m_timeHandle = gcnew PlaybackHandle(m_playController);
		}

		PlaybackNode::~PlaybackNode()
		{
		}
		PlaybackHandle ^ PlaybackNode::getTimeHandle()
		{
			return m_timeHandle;
		}
		thomas::graphics::animation::AnimationNode * PlaybackNode::Native()
		{
			return m_node;
		}
	}
}