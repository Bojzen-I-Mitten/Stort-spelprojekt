#include "Ragdoll.h"
#include "../../GameObject.h"
#include "../RenderSkinnedComponent.h"
#include "../../../graphics/animation/IBlendTree.h"
#include "../../../Common.h"
#include <stdint.h>
#include "../../../editor/gizmos/Gizmos.h"
#include <utility>

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


			void Ragdoll::OnDrawGizmos()
			{
				if (m_bodies[BodyParts::BodyPart_Chest] != nullptr)
				{

					btTransform t;
					for(int i = 0; i < Ragdoll::BodyParts::BodyParts_Amount; i++)
					{
						m_bodies[i]->getMotionState()->getWorldTransform(t);
						math::Matrix m = math::CreateMatrix(Physics::ToSimple(t.getOrigin()), Physics::ToSimple(t.getRotation()), m_gameObject->m_transform->GetScale());
						editor::Gizmos::SetColor(math::Color(0, 1, 0));
						editor::Gizmos::SetMatrix(m);
						editor::Gizmos::DrawBoundingCapsule(math::Vector3(0,0,0), boneCapsuls[i].y, m_lengths[i]);
					}
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
				
			}

			btConeTwistConstraint * Ragdoll::CreateConstraints(btRigidBody * firstbody, btRigidBody * secondbody, btTransform & firstTransform, btTransform & secondTransform,btScalar Swing1,btScalar Swing2,btScalar twist)
			{
				btConeTwistConstraint* newConeTwist =  new btConeTwistConstraint(*firstbody, *secondbody, firstTransform, secondTransform);
				newConeTwist->setLimit(Swing1, Swing2, twist);
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
				for (uint32_t i = 0; i < Ragdoll::BodyParts::BodyParts_Amount; i++) {
					m_shapes[i] = PrepareCapsulTransform(boneArr[boneConnections[i].first], boneArr[boneConnections[i].second], boneCapsuls[i].x, boneCapsuls[i].y, transform,boneCapsuls[i].z);
					m_bodies[i] = LocalCreateRigidBody(btScalar(1), transform, m_shapes[i]);
				}
				btTransform First;
				btTransform Second;
				First.setOrigin(btVector3(0, 0, 0));
				Second.setOrigin(btVector3(0.15, 0.15, 0.15));
	//			m_bodies[BodyPart_Chest]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Head]->getMotionState()->getWorldTransform(Second);
		//		First.setRotation(
				// Swing 1 Angle of the cone opening in first direction, Swing 2 Angle of the cone opening in second direction, Twist Maximum twist angle (twist)
				
				//body head
				m_joints[Joint_Neck] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Head], First, Second,60,30,120); 
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Neck], false);
			
				//body leftarm
	//			m_bodies[BodyPart_Chest]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Left_UpperArm]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Left_Arm] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Left_UpperArm], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Left_Arm], false);
					
				//left upper and left lower arm
	//			m_bodies[BodyPart_Left_UpperArm]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Left_LowerArm]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Lower_Left_Arm] = CreateConstraints(m_bodies[BodyPart_Left_UpperArm], m_bodies[BodyPart_Left_LowerArm], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Left_Arm], false);

				//body right arm
	//			m_bodies[BodyPart_Chest]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Right_UpperArm]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Right_Arm] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Right_UpperArm], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Right_Arm], false);

				//uper right arm and lower right arm
	//			m_bodies[BodyPart_Right_UpperArm]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Right_LowerArm]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Lower_Right_Arm] = CreateConstraints(m_bodies[BodyPart_Right_UpperArm], m_bodies[BodyPart_Right_LowerArm], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Right_Arm], false);

				//body rightleg
	//			m_bodies[BodyPart_Chest]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Right_UpperLeg]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Right_Leg] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Right_UpperLeg], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Right_Leg], false);

				//body leftleg
	//			m_bodies[BodyPart_Chest]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Left_UpperLeg]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Left_Leg] = CreateConstraints(m_bodies[BodyPart_Chest], m_bodies[BodyPart_Left_UpperLeg], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Left_Leg], false);

				//lower leftleg
	//			m_bodies[BodyPart_Left_UpperLeg]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Left_LowerLeg]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Lower_Left_Leg] = CreateConstraints(m_bodies[BodyPart_Left_UpperLeg], m_bodies[BodyPart_Left_LowerLeg], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Left_Leg], false);


				//lower rightleg
	//			m_bodies[BodyPart_Right_UpperLeg]->getMotionState()->getWorldTransform(First);
	//			m_bodies[BodyPart_Right_LowerLeg]->getMotionState()->getWorldTransform(Second);
				m_joints[Joint_Lower_Right_Leg] = CreateConstraints(m_bodies[BodyPart_Right_UpperLeg], m_bodies[BodyPart_Right_LowerLeg], First, Second, 60, 30, 120);
				thomas::Physics::s_world->addConstraint(m_joints[Joint_Lower_Right_Leg], false);
			}
		}
	}
}
