#pragma once
#include "Component.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Transform: public Component
			{
			private:
				void Decompose();
				/* Recreate the matrix from current S*R*T */
				void UpdateLocalMatrix();
			public:
				
					
				

				Transform();
				
				math::Vector3 Forward();
				math::Vector3 Up();
				math::Vector3 Right();

				math::Matrix GetLocalWorldMatrix();
				void SetLocalMatrix(math::Matrix matrix);
				math::Matrix GetWorldMatrix();
				void SetWorldMatrix(math::Matrix matrix);

				void LookAt(Transform* target);
				void LookAt(math::Vector3 target);
				void LookAt(math::Vector3 eye, math::Vector3 target);
				void LookAt(math::Vector3 eye, math::Vector3 target, math::Vector3 up);
				void Orient(math::Vector3 forward, math::Vector3 up);
				void Rotate(math::Vector3 angles);
				void Rotate(math::Quaternion rot);
				void Rotate(float x, float y, float z);
				void RotateByAxis(math::Vector3 axis, float angle);
				void Translate(math::Vector3 translation);
				void Translate(float x, float y, float z);


				math::Vector3 GetPosition();
				math::Quaternion GetRotation();
				math::Vector3 GetEulerAngles();
				math::Vector3 GetScale();

				void SetPosition(math::Vector3 position);
				void SetPosition(float x, float y, float z);
				void SetRotation(math::Quaternion rotation);
				void SetRotation(float yaw, float pitch, float roll);
				void SetScale(math::Vector3 scale);
				void SetScale(float x, float y, float z);
				void SetScale(float scale);


				void SetLocalPosition(math::Vector3 position);
				void SetLocalPosition(float x, float y, float z);
				void SetLocalRotation(math::Quaternion rotation);
				void SetLocalRotation(float yaw, float pitch, float roll);
				void SetLocalScale(math::Vector3 scale);
				void SetLocalScale(float x, float y, float z);
				void SetLocalScale(float scale);


				math::Vector3 GetLocalPosition();
				math::Quaternion GetLocalRotation();
				math::Vector3 GetLocalEulerAngles();
				math::Vector3 GetLocalScale();


				//void UpdateChildren();
				void SetParent(Transform* parent, bool worldPositionStays);
				Transform* GetParent();
				std::vector<Transform*> GetChildren();
				void RemoveParent();
				void OnDestroy();
				void SetDirty(bool dirty);
				bool IsDirty();
			private:

				bool m_dirty;
				Transform* m_parent;

				math::Quaternion m_localRotation;
#ifdef _EDITOR
				math::Vector3 m_euler;
#endif
				math::Vector3 m_localPosition;
				math::Vector3 m_localScale;
				math::Matrix m_localWorldMatrix;
				std::vector<Transform*> m_children;
			};
		}
	}
}