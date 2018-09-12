#pragma once
#include <vector>
#include <math.h>
#include "../../object/component/Transform.h"
#include "data/Bone.h"
#include "data/Skeleton.h"
#include "data/BoneFrame.h"
#include "../../utils/Math.h"

namespace thomas {
	namespace animation {

		class AnimationNode;

		class AnimatedSkeleton
		{
		public:
			enum PlayType
			{
				None = 0,
				Once = 1,
				Loop = 2
			};
		private:
			Skeleton& _ref;
			AnimationNode *_root;

			PlayType _endState;

			/* Bone transform in model space
			*/
			std::vector<math::Matrix> _pose;
			/* Skin Transforms in model space
			*/
			std::vector<math::Matrix> _skinTransform;

		public:
			AnimatedSkeleton(Skeleton& ref);
			~AnimatedSkeleton();
			
			void update(float dT);

			void updateSkeleton();
			/* Remove the current animation. 
			*/
			void stopAnimation();
			/* Get skin matrixes
			*/
			const std::vector<math::Matrix>& getSkin() const;
			/* Get the number of bones
			*/
			unsigned int boneCount() const;

			/* Get the world matrix of a bone
			*/
			const math::Matrix& getBoneMatrix(unsigned int bone) const;
		};
	}
}
