#pragma once
#include <vector>
#include <map>
#include <memory>
#include "Bone.h"
#include "AnimationData.h"
#include "TransformComponents.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			/* The skeleton consists of the bone data loaded into the game.
			*/
			class Skeleton
			{
				/* Bone structure of the skeleton.
				*/
				std::vector<Bone> m_bones;
				std::vector<TransformComponents> m_components;
				math::Matrix m_root;

			public:
				Skeleton(std::vector<Bone> &m_boneInfo, std::vector<TransformComponents> &bindPose, math::Matrix root);
				~Skeleton();

				const std::vector<Bone>& getBones() const;
				const Bone& getBone(unsigned int i) const;

				const TransformComponents* getBindComponents();
				/* Access a bone from name. Returns if bone name exists.
				boneName	<<	Name matched with the bones.
				boneID		>>	Returned bone index if found.
				return		>>	Returns true if bone was found.
				*/
				bool findBoneIndex(const std::string &boneName, unsigned int &boneID) const;
				/* Access a bone from hash. Returns if bone exists.
				boneHash	<<	Hashed bone name.
				boneID		>>	Returned bone index if found.
				return		>>	Returns true if bone was found.
				*/
				bool findBoneIndex(unsigned long boneHash, unsigned int &boneID) const;
				/* Get the number of bones in the skeleton */
				unsigned int getNumBones() const;
				/* Get root transform
				*/
				math::Matrix getRoot();
			};
		}
	}
}
