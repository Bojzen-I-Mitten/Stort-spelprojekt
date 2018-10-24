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
				m_twist = 1.0f;
				m_swing1 = 10.0f;
				m_swing2 = 10.0f;
				m_collision = false;
				m_damping = 0;
			}
			Joint::~Joint()
			{

			}
			#pragma region  SetFunctions
			void Joint::SetConnectedBody(Rigidbody * target)
			{
				m_connectedBody = target;
			}

			void Joint::SetFrameAAnchor(math::Vector3 value)
			{
				m_frameA.setOrigin(Physics::ToBullet(value));
				UpdateFrames();
			}

			void Joint::SetFrameAAxis(math::Vector3 value)
			{
				m_frameAAxis = value;
				UpdateFrames();
			}

			void Joint::SetFrameBAnchor(math::Vector3 value)
			{
				m_frameB.setOrigin(Physics::ToBullet(value));
				UpdateFrames();
			}

			void Joint::SetFrameBAxis(math::Vector3 value)
			{
				m_frameBAxis = value;
				UpdateFrames();
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
				m_collision = value;
				
			}

			void Joint::SetDamping(float value)
			{
				m_damping = value;
				UpdateDamping();
			}
#pragma endregion
			#pragma region GetFunctions
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
				return m_collision;
			}

			float Joint::GetDamping()
			{
				return m_damping;
			}
#pragma endregion
			btConeTwistConstraint * Joint::CreateConstraints()
			{
				btRigidBody* rbA = m_gameObject->GetComponent<Rigidbody>();
				btRigidBody* rbB = m_connectedBody;

				m_coneTwistConstraint = new btConeTwistConstraint(*rbA, *rbB, m_frameA, m_frameB);
				return m_coneTwistConstraint;
			}
			#pragma region UpdateFunctions
			void Joint::UpdateLimits()
			{
				if (m_coneTwistConstraint) 
				{
					m_coneTwistConstraint->setLimit(math::DegreesToRadians(m_swing1), math::DegreesToRadians(m_swing2), math::DegreesToRadians(m_twist));
				}
			}

			void Joint::UpdateFrames()
			{
				if (m_coneTwistConstraint) 
				{
					math::Vector3 swingA = m_frameAAxis;
					math::Vector3 swingB = m_frameBAxis;
					btQuaternion frameASwing = btQuaternion(math::DegreesToRadians(swingA.y), math::DegreesToRadians(swingA.x), math::DegreesToRadians(swingA.z));
					btQuaternion frameBSwing = btQuaternion(math::DegreesToRadians(swingB.y), math::DegreesToRadians(swingB.x), math::DegreesToRadians(swingB.z));

					m_frameA.setRotation(frameASwing);
					m_frameB.setRotation(frameBSwing);
					m_coneTwistConstraint->setFrames(m_frameA, m_frameB);
				}
			}

			void Joint::UpdateDamping()
			{
				if (m_coneTwistConstraint)
				{
					m_coneTwistConstraint->setDamping(m_damping);
				}
			}

			void Joint::UpdateLimitsFramesDamping()
			{	
				UpdateFrames();
				UpdateLimits();
				UpdateDamping();
			}

			void Joint::Update()
			{
				
				
			}
			#pragma endregion
			void Joint::Awake()
			{
			
			}
			void Joint::OnEnable()
			{
				if (m_gameObject->GetComponent<Rigidbody>() != nullptr && m_connectedBody)
				{
					m_gameObject->GetComponent<Rigidbody>()->SetActivationState(ActivationState::Always_Active);
					thomas::Physics::s_world->addConstraint(CreateConstraints(), m_collision);
					UpdateLimitsFramesDamping();
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

