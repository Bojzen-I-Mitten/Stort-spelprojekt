#pragma once

#pragma managed
#include "../../Utility.h"
#include "../../resource/Font.h"

using namespace System;

namespace ThomasEngine
{
	ref class Model;
	public ref class Gizmos
	{
	public:	
		// Gizmos, move to own class
		static void DrawModel(Model^ model, int meshIndex, Vector3 position,Quaternion rotation, Vector3 scale);
		static void DrawModel(Model^ model, Vector3 position, Quaternion rotation,Vector3 scale);
		static void DrawWireModel(Model^ model, int meshIndex,Vector3 position, Quaternion rotation, Vector3 scale);
		static void DrawWireModel(Model^ model, Vector3 position, Quaternion rotation, Vector3 scale);
		static void DrawCube(Vector3 center, Vector3 size);

		static void DrawWireCube(Vector3 center, Vector3 size);
		static void DrawBoundingBox(Vector3 center, Vector3 extends);
		static void DrawBoundingSphere(const BoundingSphere % sphere);

		static void DrawRing(Vector3 origin, Vector3 majorAxis, Vector3 minorAxis);

		static void DrawLine(Vector3 from, Vector3 to);
		static void DrawSphere(Vector3 center, float radius);
		static void DrawWireSphere(Vector3 center, float radius);
		static void DrawRay(Vector3 from, Vector3 direction);
		static void DrawRay(const Ray% ray);
		static void DrawRay(Vector3 from, Vector3 direction, float len);

		static void DrawFrustum(Vector3 center, float fov, float maxRange, float minRange, float aspect);

		static void SetColor(Color color);
		static void SetMatrix(Matrix matrix);
		static void DrawPing(String^ ping);

		static void ImguiStringUpdate(String^ text, Vector2 size, Vector2 pos);
		static void ImguiStringUpdate(String^ text, Vector2 pos);
		//
	};
}
