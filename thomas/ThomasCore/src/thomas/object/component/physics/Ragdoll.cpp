#include "Ragdoll.h"
#include <utility>
#include <stdint.h>
#include "../../GameObject.h"
#include "../RenderSkinnedComponent.h"
#include "../../../graphics/animation/IBlendTree.h"
#include "../../../Common.h"
#include "../../../editor/gizmos/Gizmos.h"
#include "../../../ThomasCore.h"

namespace thomas 
{
	namespace object 
	{
		namespace component 
		{

			Ragdoll::Ragdoll()
			{
				m_bodies[0] = nullptr;
				initvectors();
				//init();
			}

			Ragdoll::~Ragdoll()
			{
			}

			void Ragdoll::Update()
			{
				if (!m_tree)
					return;
				thomas::object::GameObject* gObj = m_gameObject;
				math::Matrix world = gObj->m_transform->GetWorldMatrix();

				for (unsigned int i = 0; i < 15; i++)
					boneArr[i] = m_tree->getBoneMatrix(m_boneIndex[i]) * world;

				/*btTransform trans;
				btVector3 diffOrigin[10];
				btQuaternion diffRotation[10];

				for(int i = 0; i < 10; i++)
				{
					m_bodies[i]->getMotionState()->getWorldTransform(trans);
					diffOrigin[i] = prevTrans[i].getOrigin() - trans.getOrigin();
					diffRotation[i] = prevTrans[i].getRotation() - trans.getRotation();
					prevTrans[i].setRotation(trans.getRotation());
					prevTrans[i].setOrigin(trans.getOrigin());
				}*/

				//btTransform trans;
				//m_bodies[0]->getMotionState()->getWorldTransform(trans);
				//unsigned int chestIndex;
				//m_tree->getBoneIndex(Hash("mixamorig:Spine"), chestIndex);
				//math::Matrix m;
				//m = math::Matrix::CreateScale(0.01f) * math::Matrix::CreateFromQuaternion(Physics::ToSimple(trans.getRotation())) *
				//	math::Matrix::CreateTranslation(Physics::ToSimple(trans.getOrigin()));
				//m_tree->setBoneMatrix(chestIndex, m);
					

				/*
				btTransform trans;
				m_bodies[0]->getMotionState()->getWorldTransform(trans);

				math::Quaternion rotation = (math::Quaternion)trans.getRotation();
				math::Vector3 pos = (math::Vector3)trans.getOrigin();
				m_gameObject->m_transform->SetPosition(pos);
				m_gameObject->m_transform->SetRotation(rotation);
				m_gameObject->m_transform->SetDirty(true);
		//		math::Matrix world = m_gameObject->m_transform->GetWorldMatrix();
		*/
			}

			void Ragdoll::Awake()
			{
				
				Init();
				//			m_shapes[BodyPart_Chest] = new btCapsuleShape(btScalar(0.15), btScalar(0.20));
				//				btTransform trans;

				//				math::Vector3 pos = m_gameObject->m_transform->GetPosition();
				//			math::Quaternion rot = m_gameObject->m_transform->GetRotation();
				//			trans.setOrigin((btVector3&)pos);
				//			trans.setRotation((btQuaternion&)rot);
				//			m_bodies[BodyPart_Chest] = localCreateRigidBody(btScalar(1.), trans, m_shapes[BodyPart_Chest]);

			}

			// Capsul settings. Order: Length (scaler), Radius
			
			const static std::string boneNames[15]
			{
				"mixamorig:Spine",
				"mixamorig:Head",
				"mixamorig:HeadTop_End",

				"mixamorig:LeftShoulder",
				"mixamorig:LeftForeArm",
				"mixamorig:LeftHandMiddle1",

				"mixamorig:RightShoulder",
				"mixamorig:RightForeArm",
				"mixamorig:RightHandMiddle1",

				"mixamorig:RightUpLeg",
				"mixamorig:RightLeg",
				"mixamorig:RightFoot",

				"mixamorig:LeftUpLeg",
				"mixamorig:LeftLeg",
				"mixamorig:LeftFoot"
			};
			const static std::string SortedBones[52]
			{
				//Body
				"mixamorig:Hips",
				"mixamorig:Spine",
				"mixamorig:Spine1",
				"mixamorig:Spine2",
				"mixamorig:Neck",
				//head
				"mixamorig:Head",
				"mixamorig:HeadTop_End",
				"mixamorig:HeadTop_End_end",
				"mixamorig:LeftEye",
				"mixamorig:LeftEye_end",
				"mixamorig:RightEye",
				"mixamorig:RightEye_end",
				//LeftUperArm
				"mixamorig:LeftShoulder",
				"mixamorig:LeftForeArm",
				"mixamorig:LeftArm",
				"mixamorig:LeftHand",
				"mixamorig:LeftHandMiddle1",
				"mixamorig:LeftHandMiddle2",
				"mixamorig:LeftHandMiddle3",
				"mixamorig:LeftHandMiddle4",
				"mixamorig:LeftHandMiddle4_end",
				"mixamorig:LeftHandThumb1",
				"mixamorig:LeftHandThumb2",
				"mixamorig:LeftHandThumb3",
				"mixamorig:LeftHandThumb4",
				"mixamorig:LeftHandThumb4_end",
				"mixamorig:LeftHandIndex1",
				"mixamorig:LeftHandIndex2",
				"mixamorig:LeftHandIndex3",
				"mixamorig:LeftHandIndex4",
				"mixamorig:LeftHandIndex4_end",
				"",
				"",
				"",
				"",
			};

			void Ragdoll::OnDrawGizmos()
			{
				if (m_bodies[BodyParts::BodyPart_Chest] != nullptr)
				{
					//Ragdoll::BodyParts::BodyParts_Amount
					btTransform t;
					for(int i = 0; i < 6; i++)
					{
						m_bodies[i]->getMotionState()->getWorldTransform(t);
						math::Matrix m = math::CreateMatrix(Physics::ToSimple(t.getOrigin()), Physics::ToSimple(t.getRotation()), m_gameObject->m_transform->GetScale());
						editor::Gizmos::Gizmo().SetColor(math::Color(0, 1, 0));
						editor::Gizmos::Gizmo().SetMatrix(m);
						editor::Gizmos::Gizmo().DrawBoundingCapsule(math::Vector3(0,0,0), boneCapsuls[i].y, m_lengths[i]);
					}
				}
			}

			void Ragdoll::OnDisable()
			{
				for (int i = 0; i < 4; ++i)
				{
					thomas::Physics::s_world->removeConstraint(m_joints[i]);
					delete m_joints[i]; m_joints[i] = 0;
				}

				for (int i = 0; i < 5; ++i)
				{
					thomas::Physics::s_world->removeRigidBody(m_bodies[i]);

					delete m_bodies[i]->getMotionState();

					delete m_bodies[i]; m_bodies[i] = 0;
					delete m_shapes[i]; m_shapes[i] = 0;
				}
			}

			void Ragdoll::SetBoneCapsuls(math::Vector3 boneCapsule, int whichCapsule)
			{
				boneCapsuls[whichCapsule] = boneCapsule;
			}

			math::Vector3 Ragdoll::GetBoneCapsuls(int whichCapsule)
			{
				return boneCapsuls[whichCapsule];
			}

			void Ragdoll::setFirstorSecond(math::Vector3 first, int value,bool Secondbool)
			{
				btTransform NewValue;
				NewValue.setOrigin(btVector3(first.x, first.y, first.z));
					if (Secondbool)
						Second[value] = NewValue;
					else
						First[value] = NewValue;

			}

			math::Vector3 Ragdoll::getFirstorSecond(int value, bool SecondBool)
			{
			
				if (SecondBool)
					return Physics::ToSimple(Second[value].getOrigin());
				else
					return Physics::ToSimple(First[value].getOrigin());
			}

			void Ragdoll::SetTwistSpin(math::Vector3 Twist, int whichtwist)
			{
				Twistspin[whichtwist] = Twist;
			}

			math::Vector3 Ragdoll::GetTwistSpin(int whichtwist)
			{
				return Twistspin[whichtwist];
			}

	

			btRigidBody * Ragdoll::LocalCreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape)
			{
				bool isDynamic = (mass != 0.f);

				btVector3 localInertia(0, 0, 0);
				if (isDynamic)
					shape->calculateLocalInertia(mass, localInertia);

				btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
				rbInfo.m_additionalDamping = true;
				btRigidBody* body = new btRigidBody(rbInfo);

				thomas::Physics::s_world->addRigidBody(body);

				return body;
			}

			void Ragdoll::initvectors()
			{
				//for(int i= 0;i< Ragdoll::BodyParts::BodyParts_Amount;i++)
				//boneCapsuls[i] = math::Vector3(1.f, 0.1f,0);
				boneCapsuls[0] = math::Vector3(0.5, 0.12, 0);
				boneCapsuls[1] = math::Vector3(0.2, 0.08, 0);
				boneCapsuls[2] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[3] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[4] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[5] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[6] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[7] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[8] = math::Vector3(0.5, 0.05, 0);
				boneCapsuls[9] = math::Vector3(0.5, 0.05, 0);

				boneConnections[0] = std::pair<uint32_t, uint32_t>(0, 1);
				boneConnections[1] = std::pair<uint32_t, uint32_t>(1, 2);
				boneConnections[2] = std::pair<uint32_t, uint32_t>(3, 4);
				boneConnections[3] = std::pair<uint32_t, uint32_t>(4, 5);
				boneConnections[4] = std::pair<uint32_t, uint32_t>(6, 7);
				boneConnections[5] = std::pair<uint32_t, uint32_t>(7, 8);
				boneConnections[6] = std::pair<uint32_t, uint32_t>(9, 10);
				boneConnections[7] = std::pair<uint32_t, uint32_t>(10, 11);
				boneConnections[8] = std::pair<uint32_t, uint32_t>(12, 13);
				boneConnections[9] = std::pair<uint32_t, uint32_t>(13, 14);
				

			//	for (int i = 0; i < 9; i++)
			//	{
			//		First[i].setOrigin(btVector3(0, 0, 0));
			//		Second[i].setOrigin(btVector3(0.15, 0.15, 0.15));
			//		Twistspin[i]= math::Vector3(60, 60, 60);
			//	}

				// Chest
				First[BodyPart_Chest].setOrigin(btVector3(0, 0.35, 0));
				Second[BodyPart_Chest].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Chest] = math::Vector3(10, 10, 45);

				// Head
				First[BodyPart_Head].setOrigin(btVector3(0.2, 0, 0));
				Second[BodyPart_Head].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Head] = math::Vector3(10, 30, 45);

				// Left upperarm
				First[BodyPart_Left_UpperArm].setOrigin(btVector3(0, -0.27, 0));
				Second[BodyPart_Left_UpperArm].setOrigin(btVector3(0.0, 0, 0));
				Twistspin[BodyPart_Left_UpperArm] = math::Vector3(50, 50, 90);

				// Left lowerarm
				First[BodyPart_Left_LowerArm].setOrigin(btVector3(-0.2, 0, 0));
				Second[BodyPart_Left_LowerArm].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Left_LowerArm] = math::Vector3(10, 30, 45);

				// Right upperarm
				First[BodyPart_Right_UpperArm].setOrigin(btVector3(0, -0.27, 0));
				Second[BodyPart_Right_UpperArm].setOrigin(btVector3(0.0, 0, 0));
				Twistspin[BodyPart_Right_UpperArm] = math::Vector3(50, 50, 90);

				// Right lowerarm
				First[BodyPart_Right_LowerArm].setOrigin(btVector3(0, 0, 0));
				Second[BodyPart_Right_LowerArm].setOrigin(btVector3(0.3, 0, 0));
				Twistspin[BodyPart_Right_LowerArm] = math::Vector3(10, 30, 45);
/*
				// Right upperleg
				First[BodyPart_Right_UpperLeg].setOrigin(btVector3(0, -0.3, 0));
				Second[BodyPart_Right_UpperLeg].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Right_UpperLeg] = math::Vector3(50, 50, 90);

				// Right lowerleg
				First[BodyPart_Right_LowerLeg].setOrigin(btVector3(0, 0, 0));
				Second[BodyPart_Right_LowerLeg].setOrigin(btVector3(1, 0, 0));
				Twistspin[BodyPart_Right_LowerLeg] = math::Vector3(50, 50, 90);

				// Left upperleg
				First[BodyPart_Left_UpperLeg].setOrigin(btVector3(0, -0.3, 0));
				Second[BodyPart_Left_UpperLeg].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Left_UpperLeg] = math::Vector3(50, 50, 90);
/*
				// Left lowerleg
				First[BodyPart_Left_LowerLeg].setOrigin(btVector3(0, 0, 0));
				Second[BodyPart_Left_LowerLeg].setOrigin(btVector3(0, 0, 0));
				Twistspin[BodyPart_Left_LowerLeg] = math::Vector3(10, 30, 45);
				*/

			}

			btConeTwistConstraint* Ragdoll::CreateConstraints(btRigidBody * firstbody, btRigidBody * secondbody, btTransform & firstTransform, btTransform & secondTransform,btScalar Swing1,btScalar Swing2,btScalar twist)
			{
				btConeTwistConstraint* newConeTwist =  new btConeTwistConstraint(*firstbody, *secondbody, firstTransform, secondTransform);
				newConeTwist->setLimit(math::DegreesToRadians(Swing1), math::DegreesToRadians(Swing2), math::DegreesToRadians(twist),1,0.3,1);
				return newConeTwist;
			}

			unsigned int Ragdoll::Hash(const char *str)
			{
				unsigned long hash = 5381;
				int c;

				while (c = *str++)
					hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

				return hash;
			}

			btCapsuleShape* Ragdoll::PrepareCapsulTransform(const math::Matrix& fromWorld, const math::Matrix& toWorld, float lengthScale, float radius, btTransform &outTrans,float extralength)
			{
				math::Vector3 bone_pos = fromWorld.Translation();
				math::Vector3 boneNext_pos = toWorld.Translation();
				math::Vector3 up = boneNext_pos - bone_pos;

				float length = up.Length();
				if(length != 0)
				up /= length; // Normalize
				math::Vector3 pos = bone_pos + up * (length * 0.5f);
				math::Quaternion q = math::getRotationTo(math::Vector3::Up, up);
			//	q.Normalize();
				outTrans.setOrigin(reinterpret_cast<btVector3&>(pos));
				outTrans.setRotation(reinterpret_cast<btQuaternion&>(q));
				length += extralength;
				length *= lengthScale;
				m_lengths.push_back(length);
				return new btCapsuleShape(radius, length);
			}

			/* Identifies all skeleton bone indices
			*/	
			void Ragdoll::InitSkeleton() 
			{
				// Try fetch skeleton
				thomas::object::GameObject* gObj = m_gameObject;
				assert(gObj);
				RenderSkinnedComponent * model = gObj->GetComponent<RenderSkinnedComponent>();
				m_tree = model ? model->GetBlendTree() : NULL;
				if (!m_tree) {
					LOG("Rigid object initiation failed, no skeleton available.");
					return;
				}
				// Bone hashes


				bool foundBones = true;
				m_boneIndex = std::vector<uint32_t>(m_tree->boneCount());
				uint32_t i = 0;
				for (; i < 15; i++)
				{
					bool found = m_tree->getBoneIndex(Hash(boneNames[i].c_str()), m_boneIndex[i]);
					foundBones &= found;
#ifdef _DEBUG
					if (!found) {
						std::string err("Bone not found with name: " + boneNames[i]);
						LOG(err);
					}
#endif
				}
				if (!foundBones) {
					LOG("Rigid object initiation failed, not all bones found.");
					return;
				}
			}
				
			void Ragdoll::Init()
			{
				InitSkeleton();

				if (!m_tree) 
					return;

				thomas::object::GameObject* gObj = m_gameObject;
				math::Matrix world = gObj->m_transform->GetWorldMatrix();


				// Fetch bone transform in object space

				for (unsigned int i = 0; i < 15; i++)
					boneArr[i] = m_tree->getBoneMatrix(m_boneIndex[i]) * world;

				
				btTransform transform;

			

				for (uint32_t i = 0; i < 10; i++) {
					m_shapes[i] = PrepareCapsulTransform(boneArr[boneConnections[i].first], boneArr[boneConnections[i].second], boneCapsuls[i].x, boneCapsuls[i].y, transform,boneCapsuls[i].z);
					m_bodies[i] = LocalCreateRigidBody(btScalar(0.001), transform, m_shapes[i]);
					 m_bodies[i]->getMotionState()->getWorldTransform(prevTrans[i]);
				}
				
				
				
				//	boneConnections[5] = RightForarm
				//	boneConnections[6] = RightUpleg
				//	boneConnections[7] = RightLeg
				//	boneConnections[8] = LeftUpleg
				//	boneConnections[9] = leftleg
			
				int i = 1;
				float constantFactor = 0.2f;

				//	boneConnections[1] = first head;
				
				btTransform centerOfMass;
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;

				//	boneConnections[2] = first leftshoulder
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;

				//	boneConnections[3] = leftforarm
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;
			
				//	boneConnections[4] = RightShoulder
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;

				//	boneConnections[5] = RightForarm
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;

				//	boneConnections[6] = RightUpleg
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				i++;

				//	boneConnections[7] = RightLeg
				centerOfMass = m_bodies[i]->getWorldTransform();
				centerOfMass.setOrigin(btVector3(boneArr[boneConnections[i].first].Translation().x, 
										boneArr[boneConnections[i].first].Translation().y, boneArr[boneConnections[i].first].Translation().z));
				m_bodies[i]->setCenterOfMassTransform(centerOfMass);
				

				//	boneConnections[8] = LeftUpleg


				//	boneConnections[9] = leftleg

				m_bodies[1]->setGravity(btVector3(0, 30, 0));

				//body head
				m_joints[Joint_Neck] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Head], First[0], Second[0],Twistspin[0].x,Twistspin[0].y,Twistspin[0].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Neck], false);
	
				//body leftarm
				m_joints[Joint_Left_Arm] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Left_UpperArm], First[1], Second[1],Twistspin[1].x,Twistspin[1].y,Twistspin[1].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Left_Arm], false);
				
				//left upper and left lower arm
				m_joints[Joint_Lower_Left_Arm] = CreateConstraints(m_bodies[BodyPart_Left_UpperArm], m_bodies[BodyPart_Left_LowerArm], First[2], Second[2],Twistspin[2].x,Twistspin[2].y,Twistspin[2].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Left_Arm], false);
				
				//body right arm
				m_joints[Joint_Right_Arm] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Right_UpperArm], First[3], Second[3], Twistspin[3].x, Twistspin[3].y, Twistspin[3].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Right_Arm], false);

				//uper right arm and lower right arm
				m_joints[Joint_Lower_Right_Arm] = CreateConstraints(m_bodies[BodyPart_Right_UpperArm], m_bodies[BodyPart_Right_LowerArm], First[4], Second[4], Twistspin[4].x, Twistspin[4].y, Twistspin[4].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Right_Arm], false);
/*
				//body rightleg
				m_joints[Joint_Right_Leg] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Right_UpperLeg], First[5], Second[5], Twistspin[5].x, Twistspin[5].y, Twistspin[5].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Right_Leg], false);

				//lower rightleg
				m_joints[Joint_Lower_Right_Leg] = CreateConstraints(m_bodies[BodyPart_Right_UpperLeg], m_bodies[BodyPart_Right_LowerLeg], First[6], Second[6], Twistspin[6].x, Twistspin[6].y, Twistspin[6].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Right_Leg], false);

				//body leftleg
				m_joints[Joint_Left_Leg] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Left_UpperLeg], First[7], Second[7], Twistspin[7].x, Twistspin[7].y, Twistspin[7].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Left_Leg], false);

				//lower leftleg
				m_joints[Joint_Lower_Left_Leg] = CreateConstraints(m_bodies[BodyPart_Left_UpperLeg], m_bodies[BodyPart_Left_LowerLeg], First[8], Second[8], Twistspin[8].x, Twistspin[8].y, Twistspin[8].z);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Left_Leg], false);

				*/

	

			
			}
		}
	}
}
