#pragma unmanaged
#include <thomas\object\component\physics\MeshCollider.h>
#include <thomas\resource\Model.h>
#pragma managed

#include "MeshCollider.h"
#include "../../../resource/Model.h"
#include "../../../resource/Resources.h"

namespace ThomasEngine
{
	MeshCollider::MeshCollider() : Collider(new thomas::object::component::MeshCollider()) {}

	Model^ MeshCollider::mesh::get() 
	{
		thomas::resource::Model* nativeModel = ((thomas::object::component::MeshCollider*)nativePtr)->GetMesh();
		Resource^ nativeResource = Resources::FindResourceFromNativePtr(nativeModel);
		if (nativeResource)
			return (Model^)nativeResource;
		else if (nativeModel)
			return gcnew Model(nativeModel);
		else
			return nullptr;
	}

	void MeshCollider::mesh::set(Model^ value)
	{
		if (value)
			((thomas::object::component::MeshCollider*)nativePtr)->SetMesh((thomas::resource::Model*)value->m_nativePtr);
		else
			((thomas::object::component::MeshCollider*)nativePtr)->SetMesh(nullptr);
	}
}