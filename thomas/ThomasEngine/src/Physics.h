#pragma once

namespace ThomasEngine
{
	ref class Collider;
	ref class Rigidbody;
	ref class Transform;
	public value class RaycastHit
	{
	public:
		property Collider^ collider;
		property Rigidbody^ rigidbody;
		property Transform^ transform;
		property float distance;
		property Vector3 normal;
		property Vector3 point;
	};		
	public ref class Physics
	{
		bool Raycast(Vector3& origin, Vector3& direction, RaycastHit& hitInfo, float maxDistance);
		bool Raycast(Vector3& origin, Vector3& direction, RaycastHit& hitInfo);
		bool Raycast(Ray& ray, RaycastHit& hitInfo, float maxDistance);
		bool Raycast(Ray& ray, RaycastHit& hitInfo);

		String^ GetLayerName(int index);
		int GetLayerCount();
		unsigned char GetLayerMask(int index);
		unsigned char SetLayerMask(int index);
		List<int>^ MaskToLayers(unsigned char mask);
	};
}