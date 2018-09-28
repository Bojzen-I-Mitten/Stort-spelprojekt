#pragma once
#include<string>
#include<stdint.h>
#include "../../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			struct Bone {

				std::string _boneName;
				math::Matrix _invBindPose;
				math::Matrix _bindPose;
				int _boneIndex;
				uint32_t _boneHash;
				/* Parent bone */
				int _parentIndex;
				Bone()
					: _boneName(), _invBindPose(), _bindPose(), _boneIndex(-1), _parentIndex(-1)
				{
				}
				Bone(const std::string &boneName, const math::Matrix &invBindPose, math::Matrix &bindPose, int boneIndex, int parentIndex)
					: _boneName(boneName), _invBindPose(invBindPose), _bindPose(bindPose), _boneIndex(boneIndex), _parentIndex(parentIndex)
				{
				}
			};
		}
	}
}
