#include "GUI.h"
#pragma unmanaged
#include <thomas/editor/gizmos/Gizmos.h>
#pragma managed

#include "../../Utility.h"

void ThomasEngine::GUI::DrawModel(Model^ model, int meshIndex,Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::DrawModel((thomas::resource::Model*)model->m_nativePtr, meshIndex, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::GUI::DrawModel(Model^ model, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::DrawModel((thomas::resource::Model*)model->m_nativePtr,Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::GUI::DrawWireModel(Model ^ model, int meshIndex, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::DrawWireModel((thomas::resource::Model*)model->m_nativePtr,meshIndex, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}

void ThomasEngine::GUI::DrawWireModel(Model ^ model, Vector3 position, Quaternion rotation, Vector3 scale)
{
	thomas::editor::Gizmos::DrawWireModel((thomas::resource::Model*)model->m_nativePtr, Utility::Convert(position), Utility::Convert(rotation), Utility::Convert(scale));
}


void ThomasEngine::GUI::DrawCube(Vector3 center, Vector3 size)
{
	thomas::editor::Gizmos::DrawCube(Utility::Convert(center), Utility::Convert(size));

}

void ThomasEngine::GUI::DrawWireCube(Vector3 center, Vector3 size)
{
	thomas::editor::Gizmos::DrawWireCube(Utility::Convert(center), Utility::Convert(size));
}

void ThomasEngine::GUI::DrawBoundingOrientedBox(const DirectX::BoundingOrientedBox % obb)
{
	thomas::editor::Gizmos::DrawBoundingOrientedBox(obb);
}

void ThomasEngine::GUI::DrawBoundingSphere(const DirectX::BoundingSphere % sphere)
{
	thomas::editor::Gizmos::DrawBoundingSphere(sphere);
}

void ThomasEngine::GUI::DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis)
{
	thomas::editor::Gizmos::DrawRing(Utility::Convert(origin), Utility::Convert(majorAxis),Utility::Convert(minorAxis));
}

void ThomasEngine::GUI::DrawLine(Vector3 from, Vector3 to)
{
	thomas::editor::Gizmos::DrawLine(Utility::Convert(from), Utility::Convert(to));
}

void ThomasEngine::GUI::DrawSphere(Vector3 center, float radius)
{
	thomas::editor::Gizmos::DrawSphere(Utility::Convert(center), radius);
}

void ThomasEngine::GUI::DrawWireSphere(Vector3 center, float radius)
{
	thomas::editor::Gizmos::DrawWireSphere(Utility::Convert(center), radius);
}

void ThomasEngine::GUI::DrawRay(Vector3 from, Vector3 direction)
{
	thomas::editor::Gizmos::DrawRay(Utility::Convert(from), Utility::Convert(direction));
}

void ThomasEngine::GUI::DrawRay(DirectX::SimpleMath::Ray ray)
{
	thomas::editor::Gizmos::DrawRay(ray);
}

void ThomasEngine::GUI::DrawFrustum(Vector3 center, float fov, float maxRange, float minRange, float aspect)
{
	thomas::editor::Gizmos::DrawFrustum(Utility::Convert(center), fov, maxRange, minRange, aspect);
}

void ThomasEngine::GUI::DrawFrustum(DirectX::BoundingFrustum % frustrum)
{
	thomas::editor::Gizmos::DrawFrustum(frustrum);
}

void ThomasEngine::GUI::SetColor(DirectX::SimpleMath::Color color)
{
	
	thomas::editor::Gizmos::SetColor(color);
}

void ThomasEngine::GUI::SetMatrix(DirectX::SimpleMath::Matrix matrix)
{
	thomas::editor::Gizmos::SetMatrix(matrix);
}

void ThomasEngine::GUI::DrawPing(String^ ping)
{	
	thomas::editor::Gizmos::DrawPing(Utility::ConvertString(ping));
}

void ThomasEngine::GUI::ImguiStringUpdate(String^ text, Vector2 size, Vector2 pos)
{
	thomas::editor::Gizmos::ImguiStringUpdate(Utility::ConvertString(text), Utility::Convert(size), Utility::Convert(pos));
}
