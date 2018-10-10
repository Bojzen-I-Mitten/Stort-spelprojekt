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
		IK_FABRIK_Constraint::IK_FABRIK_Constraint()
		{
		}


		Vector3 IK_FABRIK_Constraint::Target::get() {
			return Utility::Convert(m_ptr->m_target);
		}
		void IK_FABRIK_Constraint::Target::set(Vector3 w) {
			m_ptr->m_target = Utility::Convert(w);
		}
		thomas::graphics::animation::IK_FABRIK_Constraint * IK_FABRIK_Constraint::Native()
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
			skinn->GetBlendTree()->addConstraint(m_ptr, boneIndex);
		}
	}
}