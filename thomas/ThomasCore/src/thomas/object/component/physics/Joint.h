#pragma once
#include "../Component.h"
#include "../../../Physics.h"
#include "Rigidbody.h"
#include "../../GameObject.h"
#include "../../../utils/Math.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Joint : public Component
			{
			public:
				Joint();
				~Joint();

			public:
				void SetRigidBody(btRigidBody * secondbody);
				void SetTransformOrigin(math::Vector3 Origin, bool firstBody);
				void SetTransformRotation(math::Vector4 Rotation, bool firstBody);
				void SetTransformTwistSpin(math::Vector3 twistSpin);

			public:
				btRigidBody * GetRigidBody();
				math::Vector3 GetTransformOrigin(bool firstBody);
				math::Vector4 GetTransformRotation(bool firstBody);
				math::Vector3 GetTransformTwistSpin();

			public:
				virtual void Update() override;
				virtual void Awake() override;
				virtual void OnDisable() override;

			private:
				btConeTwistConstraint* CreateConstraints(btRigidBody * secondbody, btTransform & firstTransform, 
														 btTransform & secondTransform, math::Vector3 swingtwist);
			
			private:
				math::Vector4 Savedrotation[2];
				btConeTwistConstraint* m_coneTwistConstraint;
				btRigidBody * m_secondBody;
				btTransform m_first;
				btTransform m_second;
				math::Vector3 m_twistSpin;
			};
		}
	}
}