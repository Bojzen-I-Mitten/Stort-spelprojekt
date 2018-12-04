#pragma once
#include <memory>
#include "data/Bone.h"

namespace thomas {

	namespace resource {
		class Animation;
	}

	namespace graphics {
		namespace animation {

			class Skeleton;
			class AnimationNode;
			class BoneConstraint;

			/* Accessible interface for the animation blend tree
			*/
			class IBlendTree {
			public:
				/* Set the root bone for the animation blend tree.
				*/
				virtual void setBlendTree(AnimationNode *blendTree) = 0;
				/* Clear the  blend tree, setting root to NULL.
				*/
				virtual void clearBlendTree() = 0;
				/* Set the root bone for the animation blend tree as a single clip
				*/
				virtual void playSingle(thomas::resource::Animation * anim) = 0;

				/* Constraints
				*/

				// Add a single constraint to a specific bone (at index)
				virtual void addConstraint(BoneConstraint * bC, uint32_t boneIndex) = 0;
				// Remove a single constraint from a specific bone (at index)
				virtual void rmvConstraint(BoneConstraint * bC, uint32_t boneIndex) = 0;

				/* Get the number of bones
				*/
				virtual unsigned int boneCount() const = 0;
				/* Get the unmodified pose transform of a bone
				*/
				virtual const math::Matrix& getBoneMatrix(unsigned int bone) const = 0;
				/* Get the bone orientation matrix with normalized basis vectors
				*/ 
				virtual math::Matrix getBoneOrientation(unsigned int bone) const = 0;
				virtual const std::string& getBoneName(unsigned int bone) const = 0;
				virtual const Bone& getBoneInfo(unsigned int bone) = 0;
				virtual bool getBoneIndex(uint32_t boneNameHash, unsigned int & boneIndex) const = 0;
			};
		}
	}
}
