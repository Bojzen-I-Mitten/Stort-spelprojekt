#include "Joint.h"
#include "../../GameObject.h"
#include "Rigidbody.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{

			Joint::Joint()
			{
				m_frameA.setOrigin(btVector3(0, 0, 0));
				m_frameA.setRotation(btQuaternion::getIdentity());
				m_frameB.setOrigin(btVector3(0, 0, 0));
				m_frameB.setRotation(btQuaternion::getIdentity());

				m_connectedBody = nullptr;
				m_frameAAxis = math::Vector3::Zero;
				m_frameBAxis = math::Vector3::Zero;
				m_twist = 0.0f;
				m_swing1 = 0.0f;
				m_swing2 = 0.0f;
				Collision = false;
			}

			Joint::~Joint()
			{

			}

			void Joint::SetConnectedBody(Rigidbody * target)
			{
				m_connectedBody = target;
			}

			void Joint::SetFrameAAnchor(math::Vector3 value)
			{
				m_frameA.setOrigin(Physics::ToBullet(value));
				UpdateLimits();
			}

			void Joint::SetFrameAAxis(math::Vector3 value)
			{
				m_frameAAxis = value;
				UpdateLimits();
			}

			void Joint::SetFrameBAnchor(math::Vector3 value)
			{
				m_frameB.setOrigin(Physics::ToBullet(value));
				UpdateLimits();
			}

			void Joint::SetFrameBAxis(math::Vector3 value)
			{
				m_frameBAxis = value;
				UpdateLimits();
			}

			void Joint::SetSwing1(float value)
			{
				m_swing1 = value;
				UpdateLimits();
			}

			void Joint::SetSwing2(float value)
			{
			
				m_swing2 = value;
				UpdateLimits();
		
			}

			void Joint::SetTwist(float value)
			{
				m_twist = value;
				UpdateLimits();
			}

			void Joint::SetCollision(bool value)
			{
				Collision = value;
				
			}

			Rigidbody * Joint::GetConnectedBody()
			{
				return m_connectedBody;
			}

			math::Vector3 Joint::GetFrameAAnchor()
			{
				return Physics::ToSimple(m_frameA.getOrigin());
			}

			math::Vector3 Joint::GetFrameAAxis()
			{
				return m_frameAAxis;
			}

			math::Vector3 Joint::GetFrameBAnchor()
			{
				return Physics::ToSimple(m_frameB.getOrigin());
			}

			math::Vector3 Joint::GetFrameBAxis()
			{
				return m_frameBAxis;
			}

			float Joint::GetSwing1()
			{
				return m_swing1;
			}

			float Joint::GetSwing2()
			{
				return m_swing2;
			}

			float Joint::GetTwist()
			{
				return m_twist;
			}

			bool Joint::GetCollision()
			{
				return Collision;
			}

			btConeTwistConstraint * Joint::CreateConstraints()
			{
				btRigidBody* rbA = m_gameObject->GetComponent<Rigidbody>();
				btRigidBody* rbB = m_connectedBody;

				m_coneTwistConstraint = new btConeTwistConstraint(*rbA, *rbB, m_frameA, m_frameB);
				return m_coneTwistConstraint;
			}

			void Joint::UpdateLimits()
			{
				if (m_coneTwistConstraint) {
					
					math::Vector3 swingA = m_frameAAxis;
					math::Vector3 swingB = m_frameBAxis;
					btQuaternion frameASwing = btQuaternion(math::DegreesToRadians(swingA.y), math::DegreesToRadians(swingA.x), math::DegreesToRadians(swingA.z));
					btQuaternion frameBSwing = btQuaternion(math::DegreesToRadians(swingB.y), math::DegreesToRadians(swingB.x), math::DegreesToRadians(swingB.z));

					m_frameA.setRotation(frameASwing);
					m_frameB.setRotation(frameBSwing);
					m_coneTwistConstraint->setFrames(m_frameA, m_frameB);
					m_coneTwistConstraint->setLimit(math::DegreesToRadians(m_swing1), math::DegreesToRadians(m_swing2), math::DegreesToRadians(m_twist));

				}

			}

			void Joint::Update()
			{
				
				
			}

			void Joint::Awake()
			{
				if (m_gameObject->GetComponent<Rigidbody>() != nullptr)
				{
					m_gameObject->GetComponent<Rigidbody>()->SetActivationState(ActivationState::Always_Active);
					thomas::Physics::s_world->addConstraint(CreateConstraints(), Collision);
					UpdateLimits();
				}
			}

			void Joint::OnDisable()
			{
				if (m_coneTwistConstraint != nullptr)
				{
					thomas::Physics::s_world->removeConstraint(m_coneTwistConstraint);
					delete m_coneTwistConstraint;
					m_coneTwistConstraint = nullptr;
				}
			}

			
		}
	}
}

