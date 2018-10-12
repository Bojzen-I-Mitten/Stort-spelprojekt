#pragma once
#include "Component.h"
#include <memory>

namespace thomas
{
	namespace graphics {
		namespace animation {
			class IBlendTree;
			class BoneConstraint;
		}
	}

	namespace object
	{
		namespace component
		{


			class BoneTransformComponent : public Component
			{
			public:
				BoneTransformComponent();
				virtual ~BoneTransformComponent();
				virtual void Update();
				virtual void OnEnable();

				void SetBoneHash(uint32_t hash);
				void SetReference(graphics::animation::IBlendTree *skel);
				int GetBoneIndex();
			protected:

				uint32_t m_boneHash;
				uint32_t m_boneIndex;

				graphics::animation::IBlendTree * m_skeleton;
				std::unique_ptr<graphics::animation::BoneConstraint> m_constraint;
			};

		}
	}
}