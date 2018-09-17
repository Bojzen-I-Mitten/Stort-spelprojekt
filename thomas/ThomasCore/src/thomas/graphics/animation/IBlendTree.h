#pragma once
#include <memory>

namespace thomas {

	namespace resource {
		class Animation;
	}

	namespace graphics {
		namespace animation {

			class Skeleton;
			class AnimationNode;

			/* Accessible interface for the animation blend tree
			*/
			class IBlendTree {
			public:
				virtual void setBlendTree(std::unique_ptr<AnimationNode> &blendTree) = 0;
				virtual void clearBlendTree() = 0;
				virtual void playSingle(thomas::resource::Animation * anim) = 0;
			};
		}
	}
}
