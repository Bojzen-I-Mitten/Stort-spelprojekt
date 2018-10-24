#pragma once

#pragma unmanaged
#include <stdint.h>
#pragma managed
#include "../Component.h"


namespace ThomasEngine
{
	ref class Model;
	ref class Material;

	public ref class RendereredInstance
	{
	private:
		unsigned int m_ID;
		Model^ m_model;
		Material^ m_material;
	public:

		property unsigned int GroupID
		{
			uint32_t get() { return m_ID; }
			void set(uint32_t id) { m_ID = id; }
		}
		property Model^ Mesh
		{
			Model^ get() { return m_model; }
			void set(Model^ mesh) { m_model = mesh; }
		}
		property Material^ Mat
		{
			Material^ get() { return m_material; }
			void set(Material^ mat) { m_material = mat; }
		}

		RendereredInstance();
	};

	public ref class InstanceRenderManager : public Component
	{
	public:

		static void UpdateInstanceRenderer(List<RendereredInstance^>^ l);
		static List<RendereredInstance^>^ GetInstanceRenderList();

		
		//[Newtonsoft::Json::JsonIgnoreAttribute]
		[DisplayNameAttribute("InstanceRendering")]
		[Newtonsoft::Json::JsonPropertyAttribute(Order = 1)]
		property List<RendereredInstance^>^ Instances
		{ 
			List<RendereredInstance^>^ get();
			void set(List<RendereredInstance^>^ value);
		}

	public:
		InstanceRenderManager();
		virtual void OnEnable()override {};
		virtual void OnDisable()override {};
		virtual void Awake()override {};
		virtual void Start()override {};
		virtual void Update()override {};

	};
}