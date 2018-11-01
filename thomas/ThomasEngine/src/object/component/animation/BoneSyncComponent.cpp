
#include "../../../Utility.h"

#include "BoneSyncComponent.h"

#pragma unmanaged

#include <thomas/object/component/EmptyComponent.h>
#include <thomas/object/GameObject.h>

#pragma managed

#include "../../GameObject.h"
#include "../Transform.h"
#include "../../../Debug.h"

namespace ThomasEngine
{

		BoneSyncComponent::BoneSyncComponent()
			: Component(new thomas::object::component::EmptyComponent()),
			m_boneHash(0), m_boneIndex(0), m_offset(), m_updateSrc(nullptr)
		{

		}



		GameObject^ BoneSyncComponent::AnimatedObject::get()
		{
			return m_skeletonSrc;
		}
		void BoneSyncComponent::AnimatedObject::set(GameObject^ value)
		{
			m_skeletonSrc = value;
		}


		Matrix BoneSyncComponent::Offset::get()
		{
			return m_offset;
		}
		void BoneSyncComponent::Offset::set(Matrix value)
		{
			m_offset = value;
		}
		System::String^ BoneSyncComponent::BoneName::get()
		{
			return m_boneName;
		}

		void BoneSyncComponent::BoneName::set(System::String^ value)
		{
			if (!value)
				return;
			m_boneName = value;
			m_boneHash = Utility::hash(value);
			refreshBoneIndex();
		}

		void BoneSyncComponent::OnParentDestroy(GameObject ^ relative)
		{
			if (m_skeletonSrc == relative) 
				m_skeletonSrc = nullptr;
		}

		void BoneSyncComponent::Awake()
		{
			refreshBoneIndex();
		}
		void BoneSyncComponent::Update()
		{
			if (!m_updateSrc) return;

			gameObject->transform->world = m_offset * m_updateSrc->GetBoneMatrix(m_boneIndex);
		}
		void BoneSyncComponent::refreshBoneIndex()
		{
			if (m_skeletonSrc) {
				m_updateSrc = m_skeletonSrc->GetComponent<RenderSkinnedComponent^>();
				uint32_t boneInd;
				if (!m_updateSrc->GetBoneIndex(m_boneHash, boneInd))
					Debug::LogWarning("BoneIndex for bone: " + m_boneName + " was not found in object: " + gameObject->Name);
				else
					m_boneIndex = boneInd;	// Set
			}
		}
}