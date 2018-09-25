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

				/* Get the number of bones
				*/
				virtual unsigned int boneCount() const = 0;
				/* Get the pose transform of a bone
				*/
				virtual const math::Matrix& getBoneMatrix(unsigned int bone) const = 0;
				virtual const std::string& getBoneName(unsigned int bone) const = 0;
				virtual bool getBoneIndex(uint32_t boneNameHash, unsigned int & boneIndex) const = 0;
			};
		}
	}
}
