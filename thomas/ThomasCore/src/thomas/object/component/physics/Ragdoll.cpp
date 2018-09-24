#include "Ragdoll.h"
#include "../../GameObject.h"
btRigidBody * thomas::object::component::Ragdoll::localCreateRigidBody(btScalar mass, const btTransform & startTransform, btCollisionShape * shape)
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

thomas::object::component::Ragdoll::Ragdoll( btScalar scale_ragdoll)
{
//	thomas::Physics::s_world;
//	m_shapes[]


	m_shapes[BodyPart_Chest] = new btCapsuleShape(btScalar(scale_ragdoll*0.15), btScalar(scale_ragdoll*0.25));
	m_shapes[BodyPart_Left_LowerArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Left_LowerLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Left_UpperArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Left_UpperLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Right_LowerArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Right_LowerLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Right_UpperArm] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	m_shapes[BodyPart_Right_UpperLeg] = new btCapsuleShape(btScalar(scale_ragdoll*0.10), btScalar(scale_ragdoll*0.20));
	btTransform transform;
	transform.setOrigin((btVector3&)(m_gameObject->m_transform->GetPosition()));
	transform.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
	m_bodies[BodyPart_Chest] = localCreateRigidBody(btScalar(0.), transform, m_shapes[BodyPart_Chest]);


}

thomas::object::component::Ragdoll::~Ragdoll()
{

}

void thomas::object::component::Ragdoll::Update()
{
	btTransform transform;
	transform.setOrigin((btVector3&)(m_gameObject->m_transform->GetPosition()));
	transform.setRotation((btQuaternion&)m_gameObject->m_transform->GetRotation());
	m_bodies[BodyPart_Chest]->setWorldTransform(transform);
}
