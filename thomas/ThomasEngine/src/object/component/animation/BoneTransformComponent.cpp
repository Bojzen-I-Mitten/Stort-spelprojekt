
#include "../../../Utility.h"

#include "BoneTransformComponent.h"

#pragma unmanaged

#include <thomas/object/component/RenderSkinnedComponent.h>
#include <thomas/object/GameObject.h>

#pragma managed

#include "../../GameObject.h"
#include "../RenderSkinnedComponent.h"

namespace ThomasEngine
{

		BoneTransformComponent::BoneTransformComponent()
			: Component(new thomas::object::component::BoneTransformComponent())
		{

		}

		thomas::object::component::BoneTransformComponent* BoneTransformComponent::Native::get() 
		{
			return (thomas::object::component::BoneTransformComponent*)nativePtr;
		}



		GameObject^ BoneTransformComponent::AnimatedObject::get()
		{
			return m_gameObject;
		}
		void BoneTransformComponent::AnimatedObject::set(GameObject^ value)
		{
			m_gameObject = value;

			thomas::object::component::RenderSkinnedComponent* comp = value->Native->GetComponent<thomas::object::component::RenderSkinnedComponent>();
			if (comp == NULL)
				Native->SetReference(NULL);
			else
				Native->SetReference(comp->GetBlendTree());
		}


		System::String^ BoneTransformComponent::BoneName::get()
		{
			return m_boneName;
		}

		void BoneTransformComponent::BoneName::set(System::String^ value)
		{
			m_boneName = value;
			uint32_t hash = Utility::hash(value);
			Native->SetBoneHash(hash);
		}
}