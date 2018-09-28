#include "BlendNode.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/data/AnimationData.h>
#include <thomas/graphics/animation/AnimPlayback.h>
#include <thomas/graphics/animation/BaseAnimationTime.h>

#pragma managed
#include "../../Debug.h"


namespace ThomasEngine
{
	namespace Script
	{
		BlendNode::BlendNode(Model ^ model)
		{

			thomas::resource::Model *m = (thomas::resource::Model*)model->m_nativePtr;
			thomas::graphics::animation::Skeleton *skel = m->GetSkeleton();
			if(!skel)
				throw gcnew System::NotSupportedException(
					"Blend node can't be created: Skeleton does not exist in model " + 
					Utility::ConvertString(m->GetName()) + 
					", can't create blend node.");
			// Construct wrapped node
			this->m_node = new thomas::graphics::animation::AnimBlender(*skel);
		}

		BlendNode::~BlendNode()
		{
			delete m_node;
		}
		
		void BlendNode::appendNode(Animation ^ anim, bool loop)
		{
			using namespace thomas::graphics::animation;
			if (!anim->get()->HasAnimation()) {
				Debug::Log("Failed to add BlendNode: Anim is null with name: " + anim->Name);
				return;
			}
			AnimationData * data = anim->get()->GetAnimation();

			std::unique_ptr<Playback> playback(new BaseAnimationTime(0.f, data->m_duration, loop ? PlayType::Loop : PlayType::Once));
			AnimationNode *node = (new AnimPlayback(m_node->m_ref, playback, *data));
			m_node->pushAnimation(node);
		}
		void BlendNode::appendNode(BlendNode ^ action)
		{
			// Push action
			m_node->pushAnimation(action->m_node);
		}


		void BlendNode::generateLinearMixer(float durationPerNode)
		{
			m_node->generateLinearMixer(durationPerNode);
		}

		thomas::graphics::animation::AnimationNode * BlendNode::getNativeNode()
		{
			return m_node;
		}

	}
}