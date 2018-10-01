#include "Joint.h"
#include "../../../utils/Math.h"

thomas::object::component::Joint::Joint()
{

}

thomas::object::component::Joint::~Joint()
{

}

btConeTwistConstraint * thomas::object::component::Joint::CreateConstraints( btRigidBody * secondbody, btTransform & firstTransform, btTransform & secondTransform, math::Vector3 swingtwist)
{


	m_coneTwistConstraint = new btConeTwistConstraint(*m_gameObject->GetComponent<Rigidbody>(), *secondbody, firstTransform, secondTransform);
	m_coneTwistConstraint->setLimit(math::DegreesToRadians(swingtwist.x), math::DegreesToRadians(swingtwist.y), math::DegreesToRadians(swingtwist.z), 1, 0.3, 1);
	return m_coneTwistConstraint;	
}

void thomas::object::component::Joint::Update()
{
	m_coneTwistConstraint->setLimit(math::DegreesToRadians(m_twistSpin.x), math::DegreesToRadians(m_twistSpin.y), math::DegreesToRadians(m_twistSpin.z), 1, 0.3, 1);
	m_coneTwistConstraint->setFrames(m_first, m_second);
}

void thomas::object::component::Joint::Awake()
{
	if (m_gameObject->GetComponent<Rigidbody>() != nullptr)
	{
		thomas::Physics::s_world->addConstraint(CreateConstraints( m_secondBody, m_first,
															m_second, m_twistSpin), false);
	}															 
}

void thomas::object::component::Joint::OnDisable()
{
	if (m_coneTwistConstraint != nullptr)
	{
		thomas::Physics::s_world->removeConstraint(m_coneTwistConstraint);
		delete m_coneTwistConstraint;
		m_coneTwistConstraint = nullptr;
	}
}

void thomas::object::component::Joint::SetRigidBody(btRigidBody * secondbody)
{
	m_secondBody = secondbody;
}

void thomas::object::component::Joint::SetTransformTwistSpin(math::Vector3 twistSpin)
{
	m_twistSpin = twistSpin;
}

void thomas::object::component::Joint::SetTransformOrigin(math::Vector3 Origin, bool firstBody)
{
	if (firstBody)
		this->m_first.setOrigin(btVector3(Origin.x, Origin.y, Origin.z));

	this->m_second.setOrigin(btVector3(Origin.x, Origin.y, Origin.z));
}

void thomas::object::component::Joint::SetTransformRotation(math::Vector4 Rotation, bool firstBody)
{
	if (firstBody)
		Savedrotation[0] = Rotation;
	else
		Savedrotation[1] = Rotation;

	if (firstBody)
		this->m_first.getBasis().setEulerZYX(Rotation.z, Rotation.y, Rotation.x);//m_first.setRotation(btQuaternion(Rotation.x, Rotation.y, Rotation.z, Rotation.w));

	this->m_second.getBasis().setEulerZYX(Rotation.z, Rotation.y, Rotation.x);//setRotation(btQuaternion(Rotation.x, Rotation.y, Rotation.z, Rotation.w));	
}

thomas::math::Vector3 thomas::object::component::Joint::GetTransformOrigin(bool firstBody)
{
	
	if (firstBody)
		return Physics::ToSimple(m_first.getOrigin());

	return Physics::ToSimple(m_second.getOrigin());
}

thomas::math::Vector4 thomas::object::component::Joint::GetTransformRotation(bool firstBody)
{
	/*
	if (firstBody)
		return math::Vector4((math::Vector4)m_first.getRotation());
	return math::Vector4((math::Vector4)m_second.getRotation());
	*/
	if (firstBody)
		return Savedrotation[0];
	return Savedrotation[1];

}

thomas::math::Vector3 thomas::object::component::Joint::GetTransformTwistSpin()
{
	return m_twistSpin;
}

btRigidBody * thomas::object::component::Joint::GetRigidBody()
{
	return m_secondBody;
}