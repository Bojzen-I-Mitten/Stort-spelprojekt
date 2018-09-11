#pragma once
#include <vector>
#include "AnimatedSkeleton.h"


namespace thomas {
	namespace animation {

		const int MAX_BONE_COUNT = 140;

		/* Part of Skeleton related to a Mesh object.
		*/
		class SkeletonPart
		{
		private:
			/* The bones of the skeleton
			*/
			std::vector<int> _bones;
		public:
			SkeletonPart();
			SkeletonPart(std::vector<int> &bones);
			~SkeletonPart();
			/* Assemble the mat4 uniform array from the specific skeleton parent
			*/
			void assemble(const AnimatedSkeleton &parent, math::Matrix* array) const;
			/* Assemble the mat4 uniform array from the specific skeleton parent, applies the modelmatrix to the calculation.
			*/
			void assemble(const AnimatedSkeleton &parent, const math::Matrix &modelMatrix, math::Matrix* array) const;
			bool hasSkeleton() const;
			unsigned int numBones() const;
		};

	}
}
