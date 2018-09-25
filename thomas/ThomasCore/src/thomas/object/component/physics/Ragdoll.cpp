#include "Ragdoll.h"
#include "../../GameObject.h"
#include "../RenderSkinnedComponent.h"
#include "../../../graphics/animation/IBlendTree.h"
#include "../../../Common.h"
#include <stdint.h>
#include "../../../editor/gizmos/Gizmos.h"
namespace thomas {
	namespace object {
		namespace component {

			btRigidBody * Ragdoll::localCreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape)
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

			uint32_t hash(const char *str)
			{
				unsigned long hash = 5381;
				int c;

				while (c = *str++)
					hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

				return hash;
			}

			btCapsuleShape* prepareCapsulTransform(const math::Matrix& fromWorld, const math::Matrix& toWorld, float radius, btTransform &outTrans)
			{

				math::Vector3 m_bone_pos = fromWorld.Translation();
				math::Vector3 m_boneNext_pos = toWorld.Translation();
				math::Vector3 m_up = m_boneNext_pos - m_bone_pos;

				float length = m_up.Length();
				math::Vector3 m_pos = m_bone_pos + m_up * (length * 0.5f);

				outTrans.setOrigin(reinterpret_cast<btVector3&>(m_pos));

				math::Quaternion q(m_up, 0.f);
				outTrans.setRotation(reinterpret_cast<btQuaternion&>(q));

				return new btCapsuleShape(radius, length);
			}

			const static std::string boneNames[Ragdoll::BodyParts::BodyParts_Amount]
			{
				"hips",
				"spine5",
				"<Name>",
				"<Name>",
				"<Name>",

			};


			/* Identifies all skeleton bone indices
			*/
			/*
			void Ragdoll::initSkeleton() {
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
				m_boneIndex = std::vector<uint32_t>(m_tree->boneCount);
				uint32_t i = 0;
				for (; i < BodyParts::BodyParts_Amount; i++)
				{
					bool found = m_tree->getBoneIndex(hash(boneNames[i].c_str()), m_boneIndex[i]);
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
			*/
			/*
			void Ragdoll::init()
			{
				if (!m_tree) 
					return;

				thomas::object::GameObject* gObj = m_gameObject;
				math::Matrix world = gObj->m_transform->GetWorldMatrix();


				// Fetch bone transform in object space

				math::Matrix boneArr[BodyParts::BodyParts_Amount];
				for (unsigned int i = 0; i < BodyParts::BodyParts_Amount; i++) {
					boneArr[i] = m_tree->getBoneMatrix(m_boneIndex[i]) * world;
				}

				constexpr float chestRad = 0.3f;

				btTransform transform;
				m_shapes[BodyPart_Chest] = prepareCapsulTransform(boneArr[0], boneArr[1], chestRad, transform);
				m_bodies[BodyPart_Chest] = localCreateRigidBody(btScalar(0.), transform, m_shapes[BodyPart_Chest]);

				//	thomas::Physics::s_world;
				//	m_shapes[]

					//setting up the body parts 
						//creating btCapsuleShapes which will capsuleate the body parts
				//	m_shapes[BodyPart_Left_LowerArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Left_LowerLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Left_UpperArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Left_UpperLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Right_LowerArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Right_LowerLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Right_UpperArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//	m_shapes[BodyPart_Right_UpperLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
				//  moving the capsuleposition to the GameObjects position(wrong should be moved to the joints position)


			}
			*/
			Ragdoll::Ragdoll()
			{
//				init();
			
			}

			Ragdoll::~Ragdoll()
			{

			}

			void Ragdoll::Update()
			{

				
				btTransform trans;
				m_bodies[0]->getMotionState()->getWorldTransform(trans);

				math::Quaternion rotation = (math::Quaternion)trans.getRotation();
				math::Vector3 pos = (math::Vector3)trans.getOrigin();
				m_gameObject->m_transform->SetPosition(pos);
				m_gameObject->m_transform->SetRotation(rotation);
				m_gameObject->m_transform->SetDirty(true);
				
			}
			void Ragdoll::Awake()
			{
				
				m_shapes[BodyPart_Chest] = new btCapsuleShape(btScalar(0.15), btScalar(0.20));
				btTransform trans;

				math::Vector3 pos = m_gameObject->m_transform->GetPosition();
				math::Quaternion rot = m_gameObject->m_transform->GetRotation();


				trans.setOrigin((btVector3&)pos);
				trans.setRotation((btQuaternion&)rot);
				m_bodies[BodyPart_Chest] = localCreateRigidBody(btScalar(1.), trans, m_shapes[BodyPart_Chest]);
				
			}
		}
	}
}
