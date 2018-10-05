#include "Skeleton.h"


namespace thomas {
	namespace graphics {
		namespace animation {

			Skeleton::Skeleton(std::vector<Bone> &m_boneInfo, std::vector<TransformComponents> &bindPose, math::Matrix root)
				: m_bones(m_boneInfo), m_components(std::move(bindPose)), m_root(root)
			{
			}


			Skeleton::~Skeleton()
			{
			}

			const Bone& Skeleton::getBone(unsigned int i) const {
				return m_bones[i];
			}
			const TransformComponents * Skeleton::getBindComponents()
			{
				return m_components.data();
			}
			const std::vector<Bone>& Skeleton::getBones() const {
				return m_bones;
			}

			bool Skeleton::findBoneIndex(const std::string &boneName, unsigned int &boneID) const
			{
				for (unsigned int i = 0; i < m_bones.size(); i++)
				{
					if (m_bones[i]._boneName == boneName)
					{
						boneID = i;
						return true;
					}
				}
				return false;
			}
				bool Skeleton::findBoneIndex(unsigned long boneHash, unsigned int &boneID) const
			{
				for (unsigned int i = 0; i < m_bones.size(); i++)
				{
					if (m_bones[i]._boneHash == boneHash) {
						boneID = i;
						return true;
					}
				}
				return false;
			}
			unsigned int Skeleton::getNumBones() const {
				return m_bones.size();
			}
			math::Matrix Skeleton::getRoot()
			{
				return m_root;
			}
		}
	}
}