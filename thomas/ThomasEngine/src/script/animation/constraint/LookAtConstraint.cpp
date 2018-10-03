#include "LookAtConstraint.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/IBlendTree.h>
#include <thomas/object/component/RenderSkinnedComponent.h>

#pragma managed
#include "../../../Debug.h"
#include "../../../object/GameObject.h"

namespace ThomasEngine
{
	namespace Script
	{

		LookAtConstraint::LookAtConstraint() {

		}

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
				comp->GetBlendTree()->addConstraint(m_ptr, boneIndex);
		}
	}
}