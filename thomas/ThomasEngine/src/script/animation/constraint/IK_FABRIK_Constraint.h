#pragma once
#pragma unmanaged
#include <thomas/graphics/animation/constraint/IK_FABRIK_Constraint.h>
#include <thomas/graphics/animation/constraint/IK_FABRIK_C_Constraint.h>
#include <thomas/object/component/RenderSkinnedComponent.h>
#pragma managed
using namespace System;
namespace ThomasEngine
{
	ref class GameObject;
	ref class RenderSkinnedComponent;
	ref class Model;

	namespace Script
	{
		/*	
		*/
		public ref class IK_FABRIK_Constraint
		{
		public:

			value struct JointParams
			{
			public:
				Vector3 rotation;		// Boundary orientation offset (rotation)
				float limit_bend;
				float limit_twist;
				float pole_influence;
				uint32_t joint_type;
				float paramA;

				property Vector3 Orientation { void set(Vector3 value) { rotation = value; } Vector3 get() { return rotation; } }
				property float Limit_bend { void set(float value) { limit_bend = value; } float get() { return limit_bend; } }
				property float Limit_twist { void set(float value) { limit_twist = value; } float get() { return limit_twist; } }
				property float Pole_Influence { void set(float value) { pole_influence = value; } float get() { return pole_influence; } }
				property float ParamA { void set(float value) { paramA = value; } float get() { return paramA; } }
				property uint32_t JointType { void set(uint32_t value) { joint_type = value; } uint32_t get() { return joint_type; } }


				thomas::graphics::animation::IK_FABRIK_C_Constraint::JointParams getParam();
			};
		public:
			IK_FABRIK_Constraint(uint32_t num_link);
			~IK_FABRIK_Constraint();

			property Vector3 Target {
				Vector3 get();
				void set(Vector3 mode);
			}
			/* Targeted orientation
			*/
			property Quaternion Orientation {
				Quaternion get();
				void set(Quaternion rot);
			}
			property Vector3 PoleTarget {
				Vector3 get();
				void set(Vector3 mode);
			}
			/* Weight over IK influence over the animated bone chain.
			*/
			property float Weight {
				float get();
				void set(float w);
			}
			/* Weight influence of the Orientation argument
			*/
			property float OrientationWeight {
				float get();
				void set(float w);
			}
			/* Pose transform for target bone before IK solver was activated. (Not updated when IK is not active)
			*/
			property Matrix LastPoseTransform
			{
				Matrix get();
			}
			property uint32_t NumLinks
			{
				uint32_t get();
			}

			property uint32_t RootBoneIndex {
				uint32_t get();
			}
			property float BoneChainLength {
				float get();
			}
			property array<JointParams>^ Joints
			{
				array<JointParams>^ get();
				void set(array<JointParams>^ value);
			}

			void apply(GameObject^ obj, uint32_t boneIndex);
			void apply(RenderSkinnedComponent^ skinn, uint32_t boneIndex);
			void apply(thomas::object::component::RenderSkinnedComponent* skinn, uint32_t boneIndex);
			/* Remove constraint. */
			void disable();
			/* Set joint parameters for bone at index index
			*/
			void setJoint(uint32_t index, JointParams% value);

			thomas::graphics::animation::IK_FABRIK_C_Constraint* Native();
		private:
			uint32_t m_num_link, m_boneIndex;
			thomas::graphics::animation::IK_FABRIK_C_Constraint* m_ptr;
			thomas::object::component::RenderSkinnedComponent* m_skinn;
			array<JointParams>^ m_joints;
		};
	}
}