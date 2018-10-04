#include "LookAtConstraint.h"

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

		LookAtConstraint::LookAtConstraint(AxisConstraint mode)
			: m_ptr(new thomas::graphics::animation::LookAtConstraint((thomas::graphics::animation::LookAtConstraint::AxisConstraint)mode))
		{		}

		LookAtConstraint::~LookAtConstraint()
		{
			delete m_ptr;
		}

		thomas::graphics::animation::LookAtConstraint * LookAtConstraint::Native()
		{
			return m_ptr;
		}

		void LookAtConstraint::apply(GameObject^ gObj, uint32_t boneIndex)
		{
			thomas::object::component::RenderSkinnedComponent* comp = gObj->Native->GetComponent<thomas::object::component::RenderSkinnedComponent>();
			if (comp == NULL)
				Debug::LogWarning("Warning! Object not skinned - LookAtConstraint can't be applied to object: " + gObj->Name);
			else if (comp->GetBlendTree()->boneCount() >= boneIndex)
				Debug::LogWarning("Warning! Bone does not exist - LookAtConstraint can't be applied to object: " + gObj->Name);
			else
				apply(comp, boneIndex);
		}
		void LookAtConstraint::apply(RenderSkinnedComponent ^ skinn, uint32_t boneIndex)
		{
			if (!skinn)
				Debug::LogWarning("Warning! Skinn parameter was null");
			else if (skinn->get()->GetBlendTree()->boneCount() < boneIndex)
				Debug::LogWarning("Warning! Bone does not exist (out of bounds) - LookAtConstraint can't be applied to object: " + skinn->gameObject->Name);
			else
				apply(skinn->get(), boneIndex);

		}
		void LookAtConstraint::apply(thomas::object::component::RenderSkinnedComponent * skinn, uint32_t boneIndex)
		{
			skinn->GetBlendTree()->addConstraint(m_ptr, boneIndex);
		}



		LookAtConstraint::AxisConstraint LookAtConstraint::Mode::get(){
			return (LookAtConstraint::AxisConstraint)m_ptr->m_axis;
		}
		void LookAtConstraint::Mode::set(LookAtConstraint::AxisConstraint m) {
			m_ptr->m_axis = (thomas::graphics::animation::LookAtConstraint::AxisConstraint)m;
		}
		float LookAtConstraint::Weight::get() {
			return m_ptr->m_weight;
		}
		void LookAtConstraint::Weight::set(float w) {
			m_ptr->m_weight = w;
		}
		Vector3 LookAtConstraint::Target::get() {
			return Utility::Convert(m_ptr->m_target);
		}
		void LookAtConstraint::Target::set(Vector3 w) {
			m_ptr->m_target = Utility::Convert(w);
		}
	}
}