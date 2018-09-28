#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/AnimBlender.h>
#pragma managed
#include "../../resource/Animation.h"
#include "../../resource/Model.h"

namespace ThomasEngine
{
	namespace Script
	{

		public ref class BlendNode {
		private:
		public:
			BlendNode(Model^model);
			~BlendNode();

			void appendNode(Animation^ anim, bool loop);
			void appendNode(BlendNode^ action);

			void generateLinearMixer(float durationPerNode);


		private:

			thomas::graphics::animation::AnimBlender* m_node;

		};
	}
}