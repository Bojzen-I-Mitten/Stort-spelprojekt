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
			{
				Decompose();
				m_parent = NULL;
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
				m_localWorldMatrix = math::Matrix::CreateWorld(eye, target - eye, m_localWorldMatrix.Up());
				
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
				m_localWorldMatrix = math::Matrix::CreateWorld(m_localPosition, forward, up);
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
#ifdef _EDITOR
				m_euler = math::ToEuler(m_localRotation);
#endif
				UpdateLocalMatrix();
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

			math::Vector3 Transform::GetPosition()
			{
				if (m_parent)
					return m_parent->GetPosition() + math::Vector3::Transform(m_localPosition, math::Matrix::CreateFromQuaternion(m_parent->GetRotation()));
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
					SetLocalPosition(position - m_parent->GetPosition());
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
				if (m_parent)
					SetLocalRotation(rotation / m_parent->GetRotation());
				else
					SetLocalRotation(rotation);
			}
			void Transform::SetRotation(float yaw, float pitch, float roll)
			{
#ifdef _EDITOR
				m_euler = math::Vector3(pitch, yaw, roll);
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
			void Transform::SetLocalRotation(float yaw, float pitch, float roll)
			{
#ifdef _EDITOR
				m_euler = math::Vector3(pitch, yaw, roll);
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
				return m_euler;
#else
				return math::ToEuler(m_localRotation);
#endif
			}

			math::Vector3 Transform::GetLocalScale()
			{
				return m_localScale;
			}

		
			void Transform::SetParent(Transform * parent, bool worldPositionStays)
			{
				if (m_parent != parent)
				{
					math::Matrix m = GetWorldMatrix();
					RemoveParent();
					m_parent = parent;
					if (m_parent) {
						m_parent->m_children.push_back(this);
						if(worldPositionStays)
							SetWorldMatrix(m);
					}
					else
					{
						SetLocalMatrix(m);
					}
				}
			}
			Transform * Transform::GetParent()
			{
				if (m_parent)
					return m_parent;

				return nullptr;
			}
			std::vector<Transform*> Transform::GetChildren()
			{
				return m_children;
			}
			void Transform::RemoveParent()
			{
				if (m_parent) //Remove from old parent
				{
					for (int i = 0; i < m_parent->m_children.size(); i++)
					{
						if (m_parent->m_children[i] == this)
						{
							m_parent->m_children[i] = NULL;
							m_parent->m_children.erase(m_parent->m_children.begin() + i);
							i -= 1;
						}
							
					}
				}
				m_parent = NULL;
			}
			void Transform::OnDestroy()
			{	
				//if (m_parent)
				//{
				//	RemoveParent();
				//}
				//for (int i = 0; i < m_children.size(); i++)
				//{
				//	GameObject::Destroy(m_children[i]->m_gameObject);
				//	//m_children.erase(m_children.begin() + i);
				//	i -= 1;
				//}
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
