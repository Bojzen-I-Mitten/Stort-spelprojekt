#pragma once
#include<string>
#include<stdint.h>
#include "../../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			struct Bone {

				int _boneIndex;
				uint32_t _boneHash;
				math::Matrix _invBindPose;
				math::Matrix _bindPose;				// 
				math::Matrix _invParentOrient;		// Rotation between the axis from the parent to 'this' bone to parent's Y axis in bind pose (originating from parent translation)
				std::string _boneName;
				/* Parent bone */
				int _parentIndex;
				Bone()
					: _boneName(), _invBindPose(), _bindPose(), _invParentOrient(), _boneIndex(-1), _parentIndex(-1)
				{
				}
				Bone(const std::string &boneName, const math::Matrix &invBindPose, math::Matrix &bindPose, math::Matrix &_invDisconnectRot, int boneIndex, int parentIndex)
					: _boneName(boneName), _invBindPose(invBindPose), _bindPose(bindPose), _boneIndex(boneIndex), _parentIndex(parentIndex)
				{
				}
			};
		}
	}
}
