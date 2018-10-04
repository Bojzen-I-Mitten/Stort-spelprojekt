#include "BoneTransformComponent.h"
#include "../../graphics/animation/AnimatedSkeleton.h"
#include "../../graphics/animation/constraint/BoneChildTransformConstraint.h"
#include "../GameObject.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			BoneTransformComponent::BoneTransformComponent()
				: m_boneIndex(-1)
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
					new  graphics::animation::BoneChildTransformConstraint(*m_gameObject->m_transform));
				if(m_skeleton->getBoneIndex(m_boneHash, m_boneIndex))
					m_skeleton->addConstraint(m_constraint.get(), m_boneIndex);
			}
			void BoneTransformComponent::SetBoneHash(uint32_t hash)
			{
				m_boneHash = hash;
			}
			void BoneTransformComponent::SetReference(graphics::animation::IBlendTree * skel)
			{
				m_skeleton = skel;
			}
			int BoneTransformComponent::GetBoneIndex()
			{
				return m_boneIndex;
			}
		}
	}
}