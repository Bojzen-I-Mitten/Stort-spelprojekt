#include "Utility.h"
#include "Physics.h"
#include "object/component/physics/Collider.h"
#pragma unmanaged
#include <thomas/object/component/physics/Collider.h>
#include <thomas/Physics.h>
#include <thomas/utils/Utility.h>
#pragma managed
namespace ThomasEngine
{
	bool Physics::Raycast(Vector3& origin, Vector3& direction, RaycastHit & hitInfo, float maxDistance)
	{
		thomas::Physics::RaycastHit nativeHitInfo;

		if (thomas::Physics::Raycast(Utility::Convert(origin), Utility::Convert(direction), nativeHitInfo, maxDistance))
		{
			hitInfo.normal = Utility::Convert(nativeHitInfo.normal);
			hitInfo.point = Utility::Convert(nativeHitInfo.point);
			hitInfo.distance = nativeHitInfo.distance;
			Collider^ collider = (Collider^)ThomasEngine::Object::Find(Utility::Convert(nativeHitInfo.collider->m_guid));
			hitInfo.collider = collider;
			hitInfo.rigidbody = collider->attachedRigidbody;
			hitInfo.transform = collider->transform;
			return true;
		}
		return false;
	}

	bool Physics::Raycast(Vector3& origin, Vector3& direction, RaycastHit& hitInfo) {
		return Raycast(origin, direction, hitInfo, FLT_MAX);
	}
	bool Physics::Raycast(Ray& ray, RaycastHit& hitInfo, float maxDistance) {
		return Raycast(ray.Position, ray.Direction, hitInfo, maxDistance);
	}
	bool Physics::Raycast(Ray& ray, RaycastHit& hitInfo) {
		return Raycast(ray, hitInfo, FLT_MAX);
	}

	List<Layer>^ Physics::GetLayers()
	{
		int count = GetCollisionLayerCount();
		System::Collections::Generic::List<Layer>^ layers = gcnew System::Collections::Generic::List<Layer>();

		for (int i = 0; i < count; i++)
		{
			Layer l;
			l.Name = GetCollisionGroup(i);
			l.collisionMask = GetCollisionMask(i);
			layers->Add(l);
			
		}
		return layers;
	}

	void Physics::SetLayers(System::Collections::Generic::List<Layer>^ layers)
	{
		if (layers) {
			for (int i = 0; i < layers->Count; i++)
			{
				SetCollisionLayer(layers[i].Name, i, layers[i].collisionMask);
			}
		}

	}


	void Physics::SetCollisionLayer(String ^ name, unsigned int group, unsigned int mask)
	{
		thomas::Physics::SetCollisionLayer(Utility::ConvertString(name), group, mask);
	}

	//control if group1 collides with with group2
	void Physics::SetGroupCollisionFlag(unsigned int group1, unsigned int group2, bool collide)
	{
		thomas::Physics::SetGroupCollisionFlag(group1, group2, collide);
	}


	int Physics::GetCollisionGroup(String^ name) {
		return thomas::Physics::GetCollisionGroup(Utility::ConvertString(name));
	}

	String^ Physics::GetCollisionGroup(unsigned int group)
	{
		return Utility::ConvertString(thomas::Physics::GetCollisionGroup(group));
	}

	unsigned int Physics::GetCollisionMask(unsigned int group)
	{
		return thomas::Physics::GetCollisionMask(group);
	}
	unsigned int Physics::GetCollisionMask(String^ name)
	{
		return thomas::Physics::GetCollisionMask(Utility::ConvertString(name));
	}

	unsigned int Physics::GetCollisionLayerCount()
	{
		return thomas::Physics::GetCollisionLayerCount();
	}

	array<String^>^ Physics::GetCollisionLayerNames()
	{
		array<String^>^ layers = gcnew array<String^>(thomas::Physics::GetCollisionLayerCount());
		for (int i = 0; i < layers->Length; i++)
		{
			layers[i] = GetCollisionGroup(i);
		}
		return layers;
	}
	array<array<bool>^>^ Physics::GetCollisionLayerValues()
	{
		array<array<bool>^>^ values = gcnew array<array<bool>^>(thomas::Physics::GetCollisionLayerCount());
		for (int i = 0; i < values->Length; i++)
		{
			unsigned int mask = thomas::Physics::GetCollisionMask(i);
			values[i] = gcnew array<bool>(values->Length);
			for (int j = 0; j < values->Length; j++)
			{
				unsigned int groupBit = thomas::Physics::GetCollisionGroupBit(j);
				values[i][j] = thomas::utility::hasFlag(mask, groupBit);
			}
		}
		return values;
	}


}
