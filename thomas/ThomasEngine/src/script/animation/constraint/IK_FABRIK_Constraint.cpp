#include "IK_FABRIK_Constraint.h"

#pragma unmanaged
#include <thomas/resource/Model.h>
#include <thomas/graphics/animation/IBlendTree.h>
#include <thomas/object/component/RenderSkinnedComponent.h>

#pragma managed
#include "../../../Debug.h"
#include "../../../object/GameObject.h"
#include "../../../object/component/RenderSkinnedComponent.h"
#include "../../../Utility.h"
#include "../../../math/MathEngine.h"


namespace ThomasEngine
{
	namespace Script
	{
		IK_FABRIK_Constraint::IK_FABRIK_Constraint(uint32_t num_link)
			: m_num_link(num_link), m_boneIndex(UINT_MAX), m_ptr(new thomas::graphics::animation::IK_FABRIK_C_Constraint(num_link)), m_skinn(nullptr),
			m_joints(gcnew array<JointParams>(num_link))
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
		Quaternion IK_FABRIK_Constraint::Orientation::get() {
			return Utility::Convert(m_ptr->m_targetOrient);
		}
		void IK_FABRIK_Constraint::Orientation::set(Quaternion rot) {
			m_ptr->m_targetOrient = Utility::Convert(rot);
		}
		float IK_FABRIK_Constraint::Weight::get() {
			return m_ptr->m_weight;
		}
		void IK_FABRIK_Constraint::Weight::set(float w) {
			m_ptr->m_weight = w;
		}
		float IK_FABRIK_Constraint::OrientationWeight::get() {
			return m_ptr->m_orientationWeight;
		}
		void IK_FABRIK_Constraint::OrientationWeight::set(float w) {
			m_ptr->m_orientationWeight = w;
		}
		Matrix IK_FABRIK_Constraint::LastPoseTransform::get() {
			return Utility::Convert(m_ptr->m_originalTransform);
		}
		uint32_t IK_FABRIK_Constraint::RootBoneIndex::get() {
			return m_ptr->getRootBoneIndex();
		}
		uint32_t IK_FABRIK_Constraint::NumLinks::get() {
			return m_num_link;
		}
		float IK_FABRIK_Constraint::BoneChainLength::get() {
			return m_ptr->getChainLength();
		}
		array<IK_FABRIK_Constraint::JointParams>^ IK_FABRIK_Constraint::Joints::get() {
			return m_joints;
		}
		void IK_FABRIK_Constraint::Joints::set(array<IK_FABRIK_Constraint::JointParams>^ value) {
			if (!value) 
				return;	// Nope
			for (uint32_t i = 0; i < m_num_link && i < value->Length; i++)
				setJoint(i, value[i]);
		}
		thomas::graphics::animation::IK_FABRIK_C_Constraint* IK_FABRIK_Constraint::Native()
		{
			return m_ptr;
		}

		void IK_FABRIK_Constraint::apply(GameObject^ gObj, uint32_t boneIndex)
		{
			thomas::object::component::RenderSkinnedComponent* comp = gObj->Native->GetComponent<thomas::object::component::RenderSkinnedComponent>();
			if (comp == NULL)
				Debug::LogWarning("Object not skinned - Constraint can't be applied to object: " + gObj->Name);
			else if (comp->GetBlendTree()->boneCount() <= boneIndex)
				Debug::LogWarning("Bone does not exist - Constraint can't be applied to object: " + gObj->Name);
			else
				apply(comp, boneIndex);
		}
		void IK_FABRIK_Constraint::apply(RenderSkinnedComponent ^ skinn, uint32_t boneIndex)
		{
			if (!skinn)
				Debug::LogWarning("Skinn parameter was null");
			else if (skinn->Native->GetBlendTree()->boneCount() < boneIndex)
				Debug::LogWarning("Bone does not exist (out of bounds) - Constraint can't be applied to object: " + skinn->gameObject->Name);
			else
				apply(skinn->Native, boneIndex);

		}
		void IK_FABRIK_Constraint::apply(thomas::object::component::RenderSkinnedComponent * skinn, uint32_t boneIndex)
		{
			thomas::graphics::animation::IBlendTree* tree = skinn->GetBlendTree();
			uint32_t joint_index = boneIndex;
			if(m_num_link == 0 || tree->getBoneInfo(joint_index)._parentIndex == 0)
			{
				Debug::LogWarning("Creating IK constraint failed: Bone chain not found.");
				return;
			}
			m_ptr->setLinkAtIndex(m_num_link - 1, thomas::graphics::animation::IK_FABRIK_C_Constraint::LinkParameter(
				joint_index, m_joints[m_num_link - 1].getParam()));

			int index = int(m_num_link) - 2;
			uint32_t count = 1;
			for (; index >= 0; index--)
			{
				joint_index = tree->getBoneInfo(joint_index)._parentIndex;
				m_ptr->setLinkAtIndex(index, thomas::graphics::animation::IK_FABRIK_C_Constraint::LinkParameter(
					joint_index, m_joints[index].getParam()));
				count++;	// Count in case of break
				if (joint_index == 0)
					break;	// Skeleton root has no parent: terminate
			}
			m_num_link = count;
			m_skinn = skinn;
			m_boneIndex = boneIndex;
			skinn->GetBlendTree()->addConstraint(m_ptr, boneIndex);
		}

		void IK_FABRIK_Constraint::disable()
		{
			if (m_skinn)
				m_skinn->GetBlendTree()->rmvConstraint(m_ptr, m_boneIndex);
		}
		void IK_FABRIK_Constraint::setJoint(uint32_t index, JointParams % value)
		{
			if (index >= m_num_link)
			{
				Debug::LogWarning("Joint constraint index out of bounds.");
				return;
			}
			m_joints[index] = value;
			m_ptr->setJointAtIndex(index, value.getParam());
		}
		thomas::graphics::animation::IK_FABRIK_C_Constraint::JointParams IK_FABRIK_Constraint::JointParams::getParam()
		{
			thomas::graphics::animation::IK_FABRIK_C_Constraint::JointParams param;
			param.limit_bend = MathEngine::DegreesToRadians(limit_bend);
			param.limit_twist = MathEngine::DegreesToRadians(limit_twist);
			param.orientation = Utility::Convert(MathEngine::CreateRotationXYZ(rotation));
			param.joint_type = joint_type;
			return param;
		}
}
}