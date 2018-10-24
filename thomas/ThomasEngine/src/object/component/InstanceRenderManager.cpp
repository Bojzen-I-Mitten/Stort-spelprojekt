#pragma unmanaged
#include <thomas/object/component/ScriptComponent.h>
#include <thomas/graphics/Renderer.h>
#pragma managed
#include "InstanceRenderManager.h"
#include "../../resource/Material.h"
#include "../../resource/Model.h"

namespace ThomasEngine
{
	void InstanceRenderManager::UpdateInstanceRenderer(List<RendereredInstance^>^ list)
	{
		thomas::graphics::Renderer::Instance()->Instancing().clearList();
		/* Set values in core*/ 
		for each(RendereredInstance^ instance in list)
		{
			if (instance->Mat && instance->Mat->m_nativePtr && 
				instance->Mesh && instance->Mesh->m_nativePtr
				&& instance->GroupID) 
				continue;	// Invalid parameters
			// Send to core:
			thomas::graphics::Renderer::Instance()->Instancing().add(instance->GroupID, instance->Mesh->Native, instance->Mat->Native);
			instance->GroupID;
			instance->Mat->Native;
			instance->Mesh->Native;
		}
	}
	List<RendereredInstance^>^ InstanceRenderManager::GetInstanceRenderList()
	{
		return gcnew List<RendereredInstance^>();
	}
	InstanceRenderManager::InstanceRenderManager()
		: Component(new thomas::object::component::ScriptComponent())
	{
	}


	List<RendereredInstance^>^ InstanceRenderManager::Instances::get() 
	{ 
		return GetInstanceRenderList(); 
	}
	void InstanceRenderManager::Instances::set(List<RendereredInstance^>^ value)
	{ 
		UpdateInstanceRenderer(value);
	}
	RendereredInstance::RendereredInstance()	: 
		m_ID(0),
		m_model(nullptr),
		m_material(nullptr)
	{
	}

}
