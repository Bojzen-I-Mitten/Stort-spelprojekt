#include "IK_FABRIK_Constraint.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/IBlendTree.h>
#include <thomas/object/component/RenderSkinnedComponent.h>

#pragma managed
#include "../../../Debug.h"
#include "../../../object/GameObject.h"
#include "../../../object/component/RenderSkinnedComponent.h"



namespace ThomasEngine
{
	namespace Script
	{
		IK_FABRIK_Constraint::IK_FABRIK_Constraint(uint32_t num_link)
			: m_num_link(num_link), m_ptr(new thomas::graphics::animation::IK_FABRIK_Constraint(num_link))
		{
		}


		IK_FABRIK_Constraint::~IK_FABRIK_Constraint()
		{
			delete m_ptr;
		}

		Vector3 IK_FABRIK_Constraint::Target::get() {
			return Utility::Convert(m_ptr->m_target);
		}
		void IK_FABRIK_Constraint::Target::set(Vector3 w) {
			m_ptr->m_target = Utility::Convert(w);
		}
		Quaternion IK_FABRIK_Constraint::Orienation::get() {
			return Utility::Convert(m_ptr->m_targetOrient);
		}
		void IK_FABRIK_Constraint::Orienation::set(Quaternion rot) {
			m_ptr->m_targetOrient = Utility::Convert(rot);
		}
		float IK_FABRIK_Constraint::Weight::get() {
			return m_ptr->m_weight;
		}
		void IK_FABRIK_Constraint::Weight::set(float w) {
			m_ptr->m_weight = w;
		}
		thomas::graphics::animation::IK_FABRIK_Constraint* IK_FABRIK_Constraint::Native()
		{
			return m_ptr;
		}



		void IK_FABRIK_Constraint::apply(GameObject^ gObj, uint32_t boneIndex)
		{
			thomas::object::component::RenderSkinnedComponent* comp = gObj->Native->GetComponent<thomas::object::component::RenderSkinnedComponent>();
			if (comp == NULL)
				Debug::LogWarning("Warning! Object not skinned - LookAtConstraint can't be applied to object: " + gObj->Name);
			else if (comp->GetBlendTree()->boneCount() >= boneIndex)
				Debug::LogWarning("Warning! Bone does not exist - LookAtConstraint can't be applied to object: " + gObj->Name);
			else
				apply(comp, boneIndex);
		}
		void IK_FABRIK_Constraint::apply(RenderSkinnedComponent ^ skinn, uint32_t boneIndex)
		{
			if (!skinn)
				Debug::LogWarning("Warning! Skinn parameter was null");
			else if (skinn->get()->GetBlendTree()->boneCount() < boneIndex)
				Debug::LogWarning("Warning! Bone does not exist (out of bounds) - LookAtConstraint can't be applied to object: " + skinn->gameObject->Name);
			else
				apply(skinn->get(), boneIndex);

		}
		void IK_FABRIK_Constraint::apply(thomas::object::component::RenderSkinnedComponent * skinn, uint32_t boneIndex)
		{
			assert(m_num_link != 0);
			thomas::graphics::animation::IBlendTree* tree = skinn->GetBlendTree();
			uint32_t joint_index = boneIndex;
			m_ptr->setLinkAtIndex(m_num_link - 1, thomas::graphics::animation::IK_FABRIK_Constraint::LinkParameter(
				joint_index));
			for (uint32_t i = m_num_link - 1; i-- > 0;) {
				joint_index = tree->getBoneInfo(joint_index)._parentIndex;
				m_ptr->setLinkAtIndex(i, thomas::graphics::animation::IK_FABRIK_Constraint::LinkParameter(
					joint_index));
			}
			skinn->GetBlendTree()->addConstraint(m_ptr, boneIndex);
		}
	}
}