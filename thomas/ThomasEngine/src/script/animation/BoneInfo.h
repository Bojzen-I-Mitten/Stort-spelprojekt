#pragma once

#pragma unmanaged
#include <thomas/graphics/animation/data/Bone.h>
#pragma managed
#include "../../Utility.h"

namespace ThomasEngine
{
	namespace Script
	{
		/* Bone information reference object
		*/
		public value struct BoneInfo
		{
		public:
			int m_boneIndex;		// Bone index
			int m_parentBoneIndex;	// Index of parent bone
			uint32_t m_boneHash;	// Bone name hash
			Matrix m_invBindPose;	// Inverse bind pose matrix
			Matrix m_bindPose;		// Bind pose matrix

		internal:
			BoneInfo(thomas::graphics::animation::Bone& bone)
			{
				SetData(bone);
			}
			/* Set the data in the struct
			*/
			void SetData(const thomas::graphics::animation::Bone& bone)
			{
				m_boneIndex = bone._boneIndex;
				m_parentBoneIndex = bone._parentIndex;
				m_boneHash = bone._boneHash;
				m_invBindPose = Utility::Convert(bone._invBindPose);
				m_bindPose = Utility::Convert(bone._bindPose);
			}
		};
	}
}