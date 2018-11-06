#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;
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


	[System::Runtime::Serialization::DataContractAttribute]
	value class Layer
	{
	public:
		[System::Runtime::Serialization::DataMemberAttribute]
		String^ Name;
		[System::Runtime::Serialization::DataMemberAttribute]
		unsigned int collisionMask;
	};
	public ref class Physics
	{
		//static Dictionary<String^, int> collisionLayers;

	internal:
		static System::Collections::Generic::List<Layer>^ GetLayers();
		static void SetLayers(System::Collections::Generic::List<Layer>^ layers);
	public:

		static bool Raycast(Vector3 origin, Vector3 direction, [Out] RaycastHit% hitInfo, float maxDistance, int collisionMask);
		static bool Raycast(Vector3 origin, Vector3 direction, [Out]RaycastHit% hitInfo);
		static bool Raycast(Ray ray, [Out] RaycastHit% hitInfo, float maxDistance, int collisionMask);
		static bool Raycast(Ray ray, [Out] RaycastHit% hitInfo);

		static void SetCollisionLayer(String^ name, int group, int mask);

		//control if group1 collides with with group2
		static void SetGroupCollisionFlag(int group1, int group2, bool collide);

		static int GetCollisionGroup(String^ name);
		static int GetCollisionGroupBit(String^ name);

		static String^ GetCollisionGroup(int group);

		static int GetCollisionMask(int group);
		static int GetCollisionMask(String^ name);

		static int GetCollisionLayerCount();

		static array<String^>^ GetCollisionLayerNames();
		static array<array<bool>^>^ GetCollisionLayerValues();
	};
}