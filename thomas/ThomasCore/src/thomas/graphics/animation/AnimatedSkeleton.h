#pragma once
#include <vector>
#include <math.h>
#include <memory>
#include "../../utils/Math.h"
#include "IBlendTree.h"
#include "data/TransformComponents.h"


namespace thomas {
	namespace graphics {
		namespace animation {

			class Skeleton;
			class AnimationNode;
			class BoneConstraint;


			constexpr uint32_t MAX_CONSTRAIN_COUNT = 3;
			struct ConstraintList {
				BoneConstraint* m_list[MAX_CONSTRAIN_COUNT + 1]; // Last is NULL

				void add(BoneConstraint* bC);
				void rmv(BoneConstraint* bC);
			};

			class AnimatedSkeleton 
				: public IBlendTree
			{
			public:
				AnimatedSkeleton(Skeleton& ref);
				~AnimatedSkeleton();

				void update(float dT);
				/* Remove the current animation.
				*/
				void stopAnimation();

			public:

				virtual void setBlendTree(AnimationNode *blendTree);
				virtual void clearBlendTree();
				virtual void playSingle(thomas::resource::Animation * anim);

				/* Get skin matrixes
				*/
				const math::Matrix* getSkin() const;
				/* Get the number of bones
				*/
				virtual unsigned int boneCount() const;
				/* Get the world matrix of a bone
				*/
				virtual const math::Matrix& getBoneMatrix(unsigned int bone) const;
				bool getBoneIndex(uint32_t boneNameHash, unsigned int & boneIndex) const;
				virtual const std::string& getBoneName(unsigned int bone) const;


				virtual void addConstraint(BoneConstraint * bC, uint32_t boneIndex);
				virtual void rmvConstraint(BoneConstraint * bC, uint32_t boneIndex);

			private:
				Skeleton& _ref;									// Reference to the mesh skeleton
				AnimationNode* _root;			// Root in the blend tree
				std::unique_ptr<TransformComponents> _frame_tmp;
				std::vector<math::Matrix> _pose;				// Bone transform in model space
				std::vector<math::Matrix> _skin;				// Skin Transforms in model space, stored in a property ready for the GPU
				std::unique_ptr<ConstraintList> m_constraint;	// Constraints applied in world space.
			private:
				void updateSkeleton();
				void clearConstraints();
				void applyConstraint(uint32_t index);
			};
		}
	}
}
