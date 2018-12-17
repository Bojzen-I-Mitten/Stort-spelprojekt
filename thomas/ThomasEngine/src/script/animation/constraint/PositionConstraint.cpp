#include "PositionConstraint.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/IBlendTree.h>
#include <thomas/object/component/RenderSkinnedComponent.h>

#pragma managed
#include "../../../Debug.h"
#include "../../../object/GameObject.h"
#include "../../../object/component/RenderSkinnedComponent.h"
#include "../../../Utility.h"
namespace ThomasEngine
{
	namespace Script
	{

		PositionConstraint::PositionConstraint()
			: m_ptr(new thomas::graphics::animation::PositionConstraint())
		{		}

		PositionConstraint::~PositionConstraint()
		{
			delete m_ptr;
		}

		thomas::graphics::animation::PositionConstraint * PositionConstraint::Native()
		{
			return m_ptr;
		}

		void PositionConstraint::apply(GameObject^ gObj, uint32_t boneIndex)
		{
			thomas::object::component::RenderSkinnedComponent* comp = gObj->Native->GetComponent<thomas::object::component::RenderSkinnedComponent>();
			if (comp == NULL)
				Debug::LogWarning("Warning! Object not skinned - LookAtConstraint can't be applied to object: " + gObj->Name);
			else if (comp->GetBlendTree()->boneCount() <= boneIndex)
				Debug::LogWarning("Warning! Bone does not exist - LookAtConstraint can't be applied to object: " + gObj->Name);
			else
				apply(comp, boneIndex);
		}
		void PositionConstraint::apply(RenderSkinnedComponent ^ skinn, uint32_t boneIndex)
		{
			if (!skinn)
				Debug::LogWarning("Warning! Skinn parameter was null");
			else if (skinn->Native->GetBlendTree()->boneCount() <= boneIndex)
				Debug::LogWarning("Warning! Bone does not exist (out of bounds) - LookAtConstraint can't be applied to object: " + skinn->gameObject->Name);
			else
				apply(skinn->Native, boneIndex);

		}
		void PositionConstraint::apply(thomas::object::component::RenderSkinnedComponent * skinn, uint32_t boneIndex)
		{
			m_boneIndex = boneIndex;
			skinn->GetBlendTree()->addConstraint(m_ptr, boneIndex);
			m_skinn = skinn;
		}

		void PositionConstraint::disable()
		{
			if (m_skinn && m_boneIndex < m_skinn->GetBlendTree()->boneCount())
				m_skinn->GetBlendTree()->addConstraint(m_ptr, m_boneIndex);
		}

		Vector3 PositionConstraint::Position::get() {
			return Utility::Convert(m_ptr->m_position);
		}
		void PositionConstraint::Position::set(Vector3 w) {
			m_ptr->m_position = Utility::Convert(w);
		}
		float PositionConstraint::Distance::get() {
			return m_ptr->m_distance;
		}
		void PositionConstraint::Distance::set(float w) {
			m_ptr->m_distance = w;
		}


	}
}