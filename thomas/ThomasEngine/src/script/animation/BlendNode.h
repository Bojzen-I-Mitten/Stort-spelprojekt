#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/AnimBlender.h>
#pragma managed
#include "../../resource/Animation.h"
#include "../../resource/Model.h"
#include "WeightHandle.h"

namespace ThomasEngine
{
	namespace Script
	{

		ref class PlaybackNode;
		ref class PlaybackHandle;

		public ref class BlendNode {
		private:
		internal:
			thomas::graphics::animation::AnimationNode* Native();
		public:
			BlendNode(Model^model);
			/* Initiate from the model with number of initiated animation slots.
			*/ 
			BlendNode(Model ^ model, uint32_t numNode);
			~BlendNode();
			/* Max blend node count
			*/
			const static uint32_t MAX_ANIMATION_BLEND_NODE = thomas::MAX_ANIMATION_BLEND_NODE;


			PlaybackNode^ appendNode(Animation^ anim, bool loop);
			void appendNode(BlendNode^ action);
			void appendNode(PlaybackNode^ action);
			void setNode(PlaybackNode^ action, uint32_t index);
			void setNode(BlendNode^ action, uint32_t index);

			void generateLinearMixer(float durationPerNode);
			/* Finalize blending node by generating a weight handle. */
			WeightHandle^ generateWeightHandle();	// Deprecated, support mode only

			WeightHandle^ getWeightHandle();

			void ResetPlayback();

			/* Get a animation node from index
			*/
			thomas::graphics::animation::AnimationNode* getNodeAtIndex(uint32_t node_index);
			/* Try get a playback handle. Returns null if node is NULL or not a playback handle
			*/
			PlaybackHandle^ tryGetPlayback(uint32_t node_index);
			
		private:

			thomas::graphics::animation::AnimBlender* m_node;
			WeightHandle^ m_weightHandle;

		};
	}
}