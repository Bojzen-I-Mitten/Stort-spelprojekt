#include "Transform.h"
#include "../GameObject.h"
#include <imgui\ImGuizmo.h>
namespace thomas
{
	namespace object
	{
		namespace component
		{

			void Transform::Decompose() {
				m_localWorldMatrix.Decompose(m_localScale, m_localRotation, m_localPosition);
#ifdef _EDITOR
				m_euler = math::ToEuler(m_localRotation);
#endif
			}
			void Transform::UpdateLocalMatrix()
			{
				math::Matrix rotMatrix = math::Matrix::CreateFromQuaternion(m_localRotation);
				m_localWorldMatrix = math::Matrix::CreateScale(m_localScale) * rotMatrix;
				m_localWorldMatrix.Translation(m_localPosition);
			}

			Transform::Transform()
				: m_parent(NULL)
			{
				Decompose();
			}
			Transform::~Transform()
			{

			}
			math::Vector3 Transform::Forward()
			{
				math::Vector3 temp = GetWorldMatrix().Forward();
				temp.Normalize();
				return temp;
			}
			math::Vector3 Transform::Up()
			{
				math::Vector3 temp = GetWorldMatrix().Up();
				temp.Normalize();
				return temp;
			}
			math::Vector3 Transform::Right()
			{
				math::Vector3 temp = GetWorldMatrix().Right();
				temp.Normalize();
				return temp;
			}

			math::Matrix Transform::GetLocalWorldMatrix()
			{
				return m_localWorldMatrix;
			}

			math::Matrix Transform::GetWorldMatrix()
			{

				if (m_parent)
					return m_localWorldMatrix * m_parent->GetWorldMatrix();
				else
					return m_localWorldMatrix;
			}
			math::Matrix Transform::GetWorldInverse()
			{
				return GetWorldMatrix().Invert();
			}

			void Transform::SetWorldMatrix(math::Matrix matrix)
			{
				if (m_parent) {
					m_localWorldMatrix = matrix * m_parent->GetWorldMatrix().Invert();
				}
				else
				{
					m_localWorldMatrix = matrix;
				}
				Decompose();
			}

			void Transform::SetLocalMatrix(math::Matrix matrix)
			{
				m_localWorldMatrix = matrix;
				Decompose();
			}

			void Transform::LookAt(Transform * target)
			{
				LookAt(target->GetPosition());
			}
			void Transform::LookAt(math::Vector3 target)
			{
				LookAt(m_localPosition, target);
			}
			void Transform::LookAt(math::Vector3 eye, math::Vector3 target)
			{
				if (target == GetPosition())
					return;
				m_localWorldMatrix = math::Matrix::CreateScale(m_localScale) * math::Matrix::CreateWorld(eye, target - eye, math::Vector3::Up);
				
				Decompose();
				SetDirty(true);
			}
			void Transform::LookAt(math::Vector3 eye, math::Vector3 target, math::Vector3 up)
			{
				if (target == GetPosition())
					return;
				m_localWorldMatrix = math::Matrix::CreateWorld(eye, target - eye, up);

				Decompose();
				SetDirty(true);
			}
			void Transform::Orient(math::Vector3 forward, math::Vector3 up)
			{
				m_localWorldMatrix = math::Matrix::CreateScale(m_localScale) * math::Matrix::CreateWorld(m_localPosition, forward, up);
				Decompose();
				SetDirty(true);
			}
			void Transform::Rotate(math::Vector3 angles)
			{
				Rotate(math::Quaternion::CreateFromYawPitchRoll(angles.x, angles.y, angles.z));
			}
			void Transform::Rotate(math::Quaternion rot)
			{
				m_localRotation = m_localRotation * rot;
//#ifdef _EDITOR
				m_euler = math::ToEuler(m_localRotation);
//#endif
				UpdateLocalMatrix();
				Decompose();
				SetDirty(true);
			}
			void Transform::Rotate(float x, float y, float z)
			{
				return Rotate(math::Vector3(x, y, z));
			}
			void Transform::RotateByAxis(math::Vector3 axis, float angle)
			{
				m_localRotation = m_localRotation * math::Quaternion::CreateFromAxisAngle(axis, angle);
				UpdateLocalMatrix();
				Decompose();
			}
			void Transform::Translate(math::Vector3 translation)
			{
				m_localPosition += translation;
				m_localWorldMatrix.Translation(m_localPosition);
				SetDirty(true);
			}
			void Transform::Translate(float x, float y, float z)
			{
				return Translate(math::Vector3(x, y, z));
			}

			void Transform::Scale(math::Vector3 scale)
			{
				SetLocalScale(m_localScale + scale);
				SetDirty(true);
			}

			void Transform::Scale(float x, float y, float z)
			{
				Scale(math::Vector3(x, y, z));
			}

			math::Vector3 Transform::GetPosition()
			{
				if (m_parent)
					return GetWorldMatrix().Translation();
				else
					return m_localPosition;
			}
			math::Quaternion Transform::GetRotation()
			{
				if (m_parent)
					return m_localRotation*m_parent->GetRotation();
				else
					return m_localRotation;
			}
			math::Vector3 Transform::GetEulerAngles()
			{
				if (m_parent)
					return GetLocalEulerAngles() * m_parent->GetEulerAngles();
				else
					return GetLocalEulerAngles();
			}
			math::Vector3 Transform::GetScale()
			{
				if (m_parent)
					return m_parent->GetScale()*m_localScale;
				else
					return m_localScale;
			}

			void Transform::SetPosition(math::Vector3 position)
			{
				if (m_parent)
				{
					SetLocalPosition(math::Vector3::Transform(position, m_parent->GetWorldInverse()));
				}
				else
					SetLocalPosition(position);
			}
			void Transform::SetPosition(float x, float y, float z)
			{
				SetPosition(math::Vector3(x, y, z));
			}
			void Transform::SetRotation(math::Quaternion rotation)
			{
				if (m_parent) {
					math::Quaternion q = m_parent->GetRotation();
					q.Inverse(q);
					SetLocalRotation(rotation * q);
				}
				else
					SetLocalRotation(rotation);
			}
			void Transform::SetRotation(float yaw, float pitch, float roll)
			{
#ifdef _EDITOR
				m_euler = math::Euler(yaw, pitch, roll);
#endif
				SetRotation(math::Quaternion::CreateFromYawPitchRoll(math::DegreesToRadians(yaw), math::DegreesToRadians(pitch), math::DegreesToRadians(roll)));
			}
			void Transform::SetScale(math::Vector3 scale)
			{
				if (m_parent)
				{
					SetLocalScale(scale / m_parent->GetScale());
				}
				else
				{
					SetLocalScale(scale);
				}
			}
			void Transform::SetScale(float x, float y, float z)
			{
				return SetScale(math::Vector3(x, y, z));
			}
			void Transform::SetScale(float scale)
			{
				return SetScale(math::Vector3(scale, scale, scale));
			}

			void Transform::SetLocalPosition(math::Vector3 position)
			{
				m_localPosition = position;
				m_localWorldMatrix.Translation(position);
			}
			void Transform::SetLocalPosition(float x, float y, float z)
			{
				SetLocalPosition(math::Vector3(x, y, z));
			}
			void Transform::SetLocalRotation(math::Quaternion rotation)
			{
				m_localRotation = rotation;
				UpdateLocalMatrix();
#ifdef _EDITOR
				m_euler = math::ToEuler(m_localRotation);
#endif
			}
			void Transform::SetLocalRotation(math::Vector3 v)
			{
				math::Euler e = math::ToEuler(v);
				SetLocalRotation(e.yaw, e.pitch, e.roll);
			}
			void Transform::SetLocalRotation(float yaw, float pitch, float roll)
			{
#ifdef _EDITOR
				m_euler = math::Euler(yaw, pitch, roll);
#endif
				m_localRotation = math::Quaternion::CreateFromYawPitchRoll(math::DegreesToRadians(yaw), math::DegreesToRadians(pitch), math::DegreesToRadians(roll));
				UpdateLocalMatrix();
			}
			void Transform::SetLocalScale(math::Vector3 scale)
			{
				m_localScale = scale;
				UpdateLocalMatrix();
			}
			void Transform::SetLocalScale(float x, float y, float z)
			{
				return SetLocalScale(math::Vector3(x, y, z));
			}
			void Transform::SetLocalScale(float scale)
			{
				return SetLocalScale(math::Vector3(scale, scale, scale));
			}


			math::Vector3 Transform::GetLocalPosition()
			{
				return m_localPosition;
			}

			math::Quaternion Transform::GetLocalRotation()
			{
				return m_localRotation;
			}

			math::Vector3 Transform::GetLocalEulerAngles()
			{
#ifdef _EDITOR
				return math::FromEuler(m_euler);
#else
				return math::ToEuler(m_localRotation);
#endif
			}

			math::Vector3 Transform::GetLocalScale()
			{
				return m_localScale;
			}

			void Transform::RemoveParent()
			{
				SetParent(nullptr, true);
			}
		
			void Transform::SetParent(Transform * parent, bool worldPositionStays)
			{
				if (parent == m_parent) return;
				// Swap parent
				math::Matrix m = GetWorldMatrix();
				m_parent = parent;
				if(worldPositionStays)
					SetWorldMatrix(m);
			}
			Transform * Transform::GetParent()
			{

				return m_parent;
			}
			void Transform::OnDestroy()
			{	
				m_parent = nullptr;
			}
			void Transform::SetDirty(bool dirty)
			{
				m_dirty = dirty;
			}
			bool Transform::IsDirty()
			{
				return m_dirty;
			}
		}
	}
}
