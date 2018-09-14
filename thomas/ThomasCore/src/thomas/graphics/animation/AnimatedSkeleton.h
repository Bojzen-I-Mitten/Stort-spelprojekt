#pragma once
#include <vector>
#include <math.h>
#include "../../utils/Math.h"
#include "../../resource/ShaderProperty/ShaderPropertyMatrix.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			class Skeleton;
			class AnimationNode;

			class AnimatedSkeleton
			{
			public:
				AnimatedSkeleton(Skeleton& ref, resource::shaderproperty::ShaderPropertyMatrixArray& skin_ref);
				~AnimatedSkeleton();

				void update(float dT);
				/* Remove the current animation.
				*/
				void stopAnimation();

			public:

				void setBlendTree(std::unique_ptr<AnimationNode> &blendTree);
				void clearBlendTree();

				/* Get skin matrixes
				*/
				const math::Matrix* getSkin() const;
				/* Get the number of bones
				*/
				unsigned int boneCount() const;
				/* Get the world matrix of a bone
				*/
				const math::Matrix& getBoneMatrix(unsigned int bone) const;

				const resource::shaderproperty::ShaderPropertyMatrixArray* getShaderProperty();

			private:
				Skeleton& _ref;												// Reference to the mesh skeleton
				std::unique_ptr<AnimationNode> _root;						// Root in the blend tree
				std::vector<math::Matrix> _pose;							// Bone transform in model space
				resource::shaderproperty::ShaderPropertyMatrixArray* _skin;	// Skin Transforms in model space, stored in a property ready for the GPU
			private:
				void updateSkeleton();
			};
		}
	}
}
