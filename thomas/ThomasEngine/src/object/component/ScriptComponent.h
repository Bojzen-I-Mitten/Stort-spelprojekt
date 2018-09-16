#pragma once


#include "../Component.h"

namespace ThomasEngine
{
	[HideInInspector]
	public ref class ScriptComponent : public Component
	{
	private:

	public:
		ScriptComponent();
		virtual void OnEnable()override {};
		virtual void OnDisable()override {};
		virtual void Awake()override {};
		virtual void Start()override {};
		virtual void Update()override {};
		virtual void OnDrawGizmosSelected() override {};
		virtual void OnDrawGizmos() override {};

	};
}