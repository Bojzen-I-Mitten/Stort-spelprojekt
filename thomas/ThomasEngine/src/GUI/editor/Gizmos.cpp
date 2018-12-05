#include "Gizmos.h"

#pragma unmanaged
#include <thomas/editor/gizmos/Gizmos.h>

#pragma managed
#include "../../resource/Model.h"
#include "../../ThomasManaged.h"

// OLD
void ThomasEngine::Gizmos::DrawModel(Model^ model, int meshIndex,Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::Gizmo().DrawModel((thomas::resource::Model*)model->m_nativePtr, meshIndex, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::Gizmos::DrawModel(Model^ model, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::Gizmo().DrawModel((thomas::resource::Model*)model->m_nativePtr,Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::Gizmos::DrawWireModel(Model ^ model, int meshIndex, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::Gizmo().DrawWireModel((thomas::resource::Model*)model->m_nativePtr,meshIndex, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::Gizmos::DrawWireModel(Model ^ model, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::Gizmo().DrawWireModel((thomas::resource::Model*)model->m_nativePtr, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::Gizmos::DrawCube(Vector3 center, Vector3 size)
{
	thomas::editor::Gizmos::Gizmo().DrawCube(Utility::Convert(center), Utility::Convert(size));

}

void ThomasEngine::Gizmos::DrawWireCube(Vector3 center, Vector3 size)
{
	thomas::editor::Gizmos::Gizmo().DrawWireCube(Utility::Convert(center), Utility::Convert(size));
}

void ThomasEngine::Gizmos::DrawBoundingBox(Vector3 center, Vector3 extends)
{
	thomas::editor::Gizmos::Gizmo().DrawBoundingOrientedBox(
		DirectX::BoundingOrientedBox(Utility::Convert(center),
		Utility::Convert(extends),
		thomas::math::Quaternion::Identity));
}

void ThomasEngine::Gizmos::DrawBoundingSphere(const BoundingSphere % sphere)
{
	thomas::editor::Gizmos::Gizmo().DrawBoundingSphere(Utility::Convert(sphere));
}

void ThomasEngine::Gizmos::DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis)
{
	thomas::editor::Gizmos::Gizmo().DrawRing(Utility::Convert(origin), Utility::Convert(majorAxis),Utility::Convert(minorAxis));
}

void ThomasEngine::Gizmos::DrawLine(Vector3 from, Vector3 to)
{
	thomas::editor::Gizmos::Gizmo().DrawLine(Utility::Convert(from), Utility::Convert(to));
}

void ThomasEngine::Gizmos::DrawSphere(Vector3 center, float radius)
{
	thomas::editor::Gizmos::Gizmo().DrawSphere(Utility::Convert(center), radius);
}

void ThomasEngine::Gizmos::DrawWireSphere(Vector3 center, float radius)
{
	thomas::editor::Gizmos::Gizmo().DrawWireSphere(Utility::Convert(center), radius);
}

void ThomasEngine::Gizmos::DrawRay(Vector3 from, Vector3 direction)
{
	thomas::editor::Gizmos::Gizmo().DrawRay(Utility::Convert(from), Utility::Convert(direction));
}

void ThomasEngine::Gizmos::DrawRay(const Ray% ray)
{
	thomas::editor::Gizmos::Gizmo().DrawRay(Utility::Convert(ray));
}

void ThomasEngine::Gizmos::DrawFrustum(Vector3 center, float fov, float maxRange, float minRange, float aspect)
{
	thomas::editor::Gizmos::Gizmo().DrawFrustum(Utility::Convert(center), fov, maxRange, minRange, aspect);
}
void ThomasEngine::Gizmos::DrawRay(Vector3 from, Vector3 direction, float len)
{
	thomas::editor::Gizmos::Gizmo().DrawRay(Utility::Convert(from), Utility::Convert(direction), len);
}


void ThomasEngine::Gizmos::SetColor(Color color)
{
	
	thomas::editor::Gizmos::Gizmo().SetColor(Utility::Convert(color));
}

void ThomasEngine::Gizmos::SetMatrix(Matrix matrix)
{
	thomas::editor::Gizmos::Gizmo().SetMatrix(Utility::Convert(matrix));
}

void ThomasEngine::Gizmos::DrawPing(String^ ping)
{	
	thomas::editor::Gizmos::Gizmo().DrawPing(Utility::ConvertString(ping));
}

void ThomasEngine::Gizmos::ImguiStringUpdate(String^ text, Vector2 size, Vector2 pos)
{
	if (ThomasEngine::ThomasWrapper::IsEditorBuild())
		thomas::editor::Gizmos::Gizmo().ImguiStringUpdate(Utility::ConvertString(text), Utility::Convert(size), Utility::Convert(pos));
}
void ThomasEngine::Gizmos::ImguiStringUpdate(String ^ text, Vector2 pos)
{
	if(ThomasEngine::ThomasWrapper::IsEditorBuild())
		thomas::editor::Gizmos::Gizmo().ImguiStringUpdate(Utility::ConvertString(text), Utility::Convert(pos));
}
//
