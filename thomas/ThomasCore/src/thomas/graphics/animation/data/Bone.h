#pragma once
#include<string>
#include<stdint.h>
#include "../../../utils/Math.h"
#include "../../../utils/Utility.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			struct Bone {

				int _boneIndex;						// Index of bone
				int _parentIndex;					// Parent bone index	(-1 if no parent)
				uint32_t _boneHash;					// Bone name as hash
				math::Matrix _invBindPose;			// Inverse bind pose transform from object space -> relative
				math::Matrix _bindPose;				// Bind pose transform relative to parent
				math::Matrix _invParentOrient;		// Rotation between the axis from the parent to 'this' bone to parent's Y axis in bind pose (originating from parent translation)
				std::string _boneName;				// Bone name string
				Bone() : 
					_boneIndex(-1), _parentIndex(-1), _boneHash(), _invBindPose(), _bindPose(), _invParentOrient(), _boneName()
				{
				}
				Bone(const std::string &boneName, const math::Matrix &invBindPose, math::Matrix &bindPose, math::Matrix &_invDisconnectRot, int boneIndex, int parentIndex) : 
					_boneIndex(boneIndex), _parentIndex(parentIndex), _boneHash(utility::hash(boneName)), _invBindPose(invBindPose), _bindPose(bindPose), _invParentOrient(), _boneName(boneName)
				{
				}
			};
		}
	}
}
