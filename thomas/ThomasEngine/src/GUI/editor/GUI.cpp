#include "GUI.h"

#pragma unmanaged
#include <thomas/editor/gizmos/Gizmos.h>
#include <thomas/graphics/gui/ThomasGUI.h>

#pragma managed
#include "../../resource/Model.h"
#include "../../ThomasManaged.h"

// OLD
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

void ThomasEngine::GUI::SetColor(Color color)
{	
	thomas::editor::Gizmos::SetColor(Utility::Convert(color));
}

void ThomasEngine::GUI::SetMatrix(Matrix matrix)
{
	thomas::editor::Gizmos::SetMatrix(Utility::Convert(matrix));
}

void ThomasEngine::GUI::DrawPing(String^ ping)
{	
	thomas::editor::Gizmos::DrawPing(Utility::ConvertString(ping));
}

void ThomasEngine::GUI::ImguiStringUpdate(String^ text, Vector2 size, Vector2 pos)
{
	if (ThomasEngine::ThomasWrapper::InEditor())
		thomas::editor::Gizmos::ImguiStringUpdate(Utility::ConvertString(text), Utility::Convert(size), Utility::Convert(pos));
}
void ThomasEngine::GUI::ImguiStringUpdate(String ^ text, Vector2 pos)
{
	if(ThomasEngine::ThomasWrapper::InEditor())
		thomas::editor::Gizmos::ImguiStringUpdate(Utility::ConvertString(text), Utility::Convert(pos));
}
//

// Images
void ThomasEngine::GUI::AddImage(String^ id, Texture2D^ texture, Vector2 position)
{
	thomas::GUI::ThomasGUI::AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position));
}

void ThomasEngine::GUI::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector4 color)
{
	thomas::GUI::ThomasGUI::AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
								     Utility::Convert(color));
}

void ThomasEngine::GUI::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale)
{
	thomas::GUI::ThomasGUI::AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
									 Utility::Convert(Vector4(1.f)), Utility::Convert(scale));
}

void ThomasEngine::GUI::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation)
{
	thomas::GUI::ThomasGUI::AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
									 Utility::Convert(Vector4(1.f)), Utility::Convert(scale), rotation);
}

void ThomasEngine::GUI::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, Vector4 color)
{
	thomas::GUI::ThomasGUI::AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
									  Utility::Convert(color), Utility::Convert(scale), rotation);
}
