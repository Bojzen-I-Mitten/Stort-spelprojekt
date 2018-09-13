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

			class AnimatedSkeleton :
				protected resource::shaderproperty::ShaderPropertyMatrixArray	// Skin Transforms in model space
			{
			public:
				AnimatedSkeleton(Skeleton& ref);
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
				Skeleton& _ref;								// Reference to the mesh skeleton
				std::unique_ptr<AnimationNode> _root;		// Root in the blend tree
				std::vector<math::Matrix> _pose;			// Bone transform in model space
			private:
				void updateSkeleton();
			};
		}
	}
}
