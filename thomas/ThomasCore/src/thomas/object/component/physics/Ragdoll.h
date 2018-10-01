#pragma once
//#include <iostream>
#include "../Component.h"
#include "../../../Physics.h"
#include "Collider.h"
#include "../../../graphics/animation/IBlendTree.h"

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
					BodyPart_Head,

					BodyPart_Left_UpperArm,
					BodyPart_Left_LowerArm,

					BodyPart_Right_UpperArm,
					BodyPart_Right_LowerArm,


					BodyPart_Right_UpperLeg,
					BodyPart_Right_LowerLeg,

					BodyPart_Left_UpperLeg,
					BodyPart_Left_LowerLeg,


					BodyParts_Amount		
				};

				enum Joint
				{
					Joint_Neck,

					Joint_Left_Arm,
					Joint_Lower_Left_Arm,

					Joint_Right_Arm,
					Joint_Lower_Right_Arm,

					Joint_Left_Leg,
					Joint_Lower_Left_Leg,

					Joint_Right_Leg,
					Joint_Lower_Right_Leg,

					Joint_Amount
				};
				
			public:
				Ragdoll();
				~Ragdoll();
				virtual void Update() override;
				virtual void Awake() override;
				virtual void OnDrawGizmos() override;
				virtual void OnDisable() override;
				void SetBoneCapsuls(math::Vector3 boneCapsule, int whichCapsule);
				math::Vector3 GetBoneCapsuls( int whichCapsule);

				void setFirstorSecond(math::Vector3 first,int value,bool Secondbool);
				math::Vector3 getFirstorSecond(int value, bool SecondBool);

				void SetTwistSpin(math::Vector3 Twist, int whichtwist);
				math::Vector3 GetTwistSpin(int whichtwist);

			private:
				void Init();
				void InitSkeleton();
				unsigned int Hash(const char *str);
				btCapsuleShape * PrepareCapsulTransform(const math::Matrix & fromWorld, const math::Matrix & toWorld, float lengthScale, float radius, btTransform & outTrans, float extralength);
				btRigidBody* LocalCreateRigidBody(btScalar mass, const btTransform& startTransform, btCollisionShape* shape);
				void initvectors();
				btConeTwistConstraint* CreateConstraints(btRigidBody * firstbody, btRigidBody * secondbody, btTransform & firstTransform, btTransform & secondTransform, btScalar Swing1, btScalar Swing2, btScalar twist);
			private:
				btTransform prevTrans[9];
				math::Matrix boneArr[15];
				btCollisionShape* m_shapes[BodyParts_Amount];
				btRigidBody* m_bodies[BodyParts_Amount];
				btConeTwistConstraint* m_joints[Joint_Amount];
				thomas::graphics::animation::IBlendTree* m_tree;
				std::vector<uint32_t> m_boneIndex;
				std::vector<float> m_lengths;
				math::Vector3 boneCapsuls[Ragdoll::BodyParts::BodyParts_Amount];
				std::pair<uint32_t, uint32_t> boneConnections[Ragdoll::BodyParts::BodyParts_Amount];
				btTransform First[9];
				btTransform Second[9];
				math::Vector3 Twistspin[9];

				
			};

		}

	}

}