#pragma once
#include "../Component.h"
#include "../../../Physics.h"
#include "../../../utils/Math.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Rigidbody;
			class Joint : public Component
			{
			public:
				Joint();
				~Joint();

			public:
				void SetConnectedBody(Rigidbody* target);

				void SetFrameAAnchor(math::Vector3 value);
				void SetFrameAAxis(math::Vector3 value);

				void SetFrameBAnchor(math::Vector3 value);
				void SetFrameBAxis(math::Vector3 value);
				void SetSwing1(float value);
				void SetSwing2(float value);
				void SetTwist(float value);
				void SetCollision(bool value);
				void SetDamping(float value);

			public:
				Rigidbody* GetConnectedBody();

				math::Vector3 GetFrameAAnchor();
				math::Vector3 GetFrameAAxis();
				
				math::Vector3 GetFrameBAnchor();
				math::Vector3 GetFrameBAxis();
				float GetSwing1();
				float GetSwing2();
				float GetTwist();
				bool GetCollision();
				float GetDamping();

			public:
				virtual void Update() override;
				virtual void Awake() override;
				virtual void OnDisable() override;

			private:
				btConeTwistConstraint* CreateConstraints();
				void UpdateLimits();
			private:
				btConeTwistConstraint* m_coneTwistConstraint = nullptr;
				Rigidbody* m_connectedBody;
				btTransform m_frameA;
				btTransform m_frameB;
				math::Vector3 m_frameAAxis;
				math::Vector3 m_frameBAxis;
				float m_swing1;
				float m_swing2;
				float m_twist;
				
				float m_damping;
				bool m_collision;

			};
		}
	}
}