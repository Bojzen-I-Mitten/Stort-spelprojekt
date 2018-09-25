#pragma once
//#include <iostream>
#include "../Component.h"
#include "../../../Physics.h"
#include "Collider.h"



namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Ragdoll : public Component
			{

			public:
				enum BodyParts
				{
				
					BodyPart_Chest,
					
					BodyPart_Left_UpperArm,
					BodyPart_Left_LowerArm,

					BodyPart_Right_UpperArm,
					BodyPart_Right_LowerArm,

					BodyPart_Left_UpperLeg,
					BodyPart_Left_LowerLeg,

					BodyPart_Right_UpperLeg,
					BodyPart_Right_LowerLeg,
					BodyParts_Amount
					
				};

				enum Joint
				{
					Joint_Neck,
					joint_pelvis,

					Joint_Left_Arm,
					Joint_Right_Arm,

					Joint_Left_Leg,
					Joint_Right_Leg,
					Joint_Amount
				};
			private:
				btCollisionShape* m_shapes[BodyParts_Amount];
				btRigidBody* m_bodies[BodyParts_Amount];
				btTypedConstraint* m_joints[Joint_Amount];
				

				btRigidBody* localCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);

//				thomas::graphics::animation::IBlendTree* m_tree;
				std::vector<uint32_t> m_boneIndex;

				void init();
				void initSkeleton();

			public:
				Ragdoll(); // tar emot objectets position och en scale på figuren. 
				~Ragdoll();
				virtual void Update() override;
				virtual void Awake() override;
			};

		}

	}

}