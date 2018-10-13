#pragma once

#pragma managed
#include "../../Utility.h"
#include "../../resource/texture/Texture2D.h"

using namespace System;

namespace ThomasEngine
{
	ref class Model;
	public ref class GUI
	{
	public:	
		// OLD
		static void DrawModel(Model^ model, int meshIndex, Vector3 position,Quaternion rotation, Vector3 scale);
		static void DrawModel(Model^ model, Vector3 position, Quaternion rotation,Vector3 scale);
		static void DrawWireModel(Model^ model, int meshIndex,Vector3 position, Quaternion rotation, Vector3 scale);
		static void DrawWireModel(Model^ model, Vector3 position, Quaternion rotation, Vector3 scale);
		static void DrawCube(Vector3 center, Vector3 size);

		static void DrawWireCube(Vector3 center, Vector3 size);
		static void DrawBoundingOrientedBox(const DirectX::BoundingOrientedBox % obb);
		static void DrawBoundingSphere(const  DirectX::BoundingSphere % sphere);
		static void DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis);

		static void DrawLine(Vector3 from, Vector3 to);
		static void DrawSphere(Vector3 center, float radius);
		static void DrawWireSphere(Vector3 center, float radius);
		static void DrawRay(Vector3 from, Vector3 direction);
		static void DrawRay( DirectX::SimpleMath::Ray ray);
		static void DrawFrustum(Vector3 center, float fov, float maxRange, float minRange, float aspect);
		static void DrawFrustum(DirectX::BoundingFrustum% frustrum);
		static void SetColor(Color color);
		static void SetMatrix(Matrix matrix);
		static void DrawPing(String^ ping);

		static void ImguiStringUpdate(String^ text, Vector2 size, Vector2 pos);
		static void ImguiStringUpdate(String^ text, Vector2 pos);
		//

		// Images
		static void AddImage(String^ id, Texture2D^ texture, Vector2 position);
		static void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector4 color);
		static void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale);
		static void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation);
		static void AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, Vector4 color);

		// Setters
		static void SetImageTexture(String^ id, Texture2D^ texture);
		static void SetImagePosition(String^ id, Vector2 position);
		static void SetImageColor(String^ id, Vector4 color);
		static void SetImageScale(String^ id, Vector2 scale);
		static void SetImageRotation(String^ id, float rotation);
		//
	};
}
