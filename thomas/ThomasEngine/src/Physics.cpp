#include "Utility.h"
#include "Physics.h"
#include "object/component/physics/Collider.h"
#pragma unmanaged
#include <thomas/object/component/physics/Collider.h>
#include <thomas/Physics.h>
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

	String ^ Physics::GetLayerName(int index)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}

	int Physics::GetLayerCount()
	{
		return ;
	}

	unsigned char Physics::GetLayerMask(int index)
	{
		return 0;
	}

	unsigned char Physics::SetLayerMask(int index)
	{
		return 0;
	}

	List<int>^ Physics::MaskToLayers(unsigned char mask)
	{
		throw gcnew System::NotImplementedException();
		// TODO: insert return statement here
	}


}
