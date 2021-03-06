#include "BoneTransformComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../graphics/animation/constraint/BoneChildTransformConstraint.h"
#include "../GameObject.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			BoneTransformComponent::BoneTransformComponent()
				: m_boneIndex(UINT32_MAX), m_skeleton(nullptr)
			{
			}
			BoneTransformComponent::~BoneTransformComponent()
			{
			}
			void BoneTransformComponent::Update()
			{
			}
			void BoneTransformComponent::OnEnable()
			{
				if (!m_skeleton) 
				{ 
					std::string err("WARNING, Core::BoneTransformComponent. No skeleton/object specified in object: ");
					err.append(m_gameObject->GetName());
					LOG(err); 
					return; 
				}
				// Execute
				m_constraint = std::unique_ptr<graphics::animation::BoneConstraint>(
					new  graphics::animation::BoneChildTransformConstraint(*m_gameObject->GetTransform()));
				if (m_skeleton->getBoneIndex(m_boneHash, m_boneIndex))
					m_skeleton->addConstraint(m_constraint.get(), m_boneIndex);
				else
					m_boneIndex = UINT32_MAX;
			}

			void BoneTransformComponent::OnDisable()
			{
				if(m_boneIndex != UINT32_MAX)
					m_skeleton->rmvConstraint(m_constraint.get(), m_boneIndex);
			}


			void BoneTransformComponent::SetBoneHash(uint32_t hash)
			{
				m_boneHash = hash;
			}
			void BoneTransformComponent::SetReference(graphics::animation::IBlendTree * skel)
			{
				m_skeleton = skel;
			}
			void BoneTransformComponent::ClearReference()
			{
				m_skeleton = NULL;
				m_boneIndex = UINT32_MAX;
			}
			int BoneTransformComponent::GetBoneIndex()
			{
				return m_boneIndex;
			}
		}
	}
}