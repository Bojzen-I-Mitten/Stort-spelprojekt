#pragma once
#pragma unmanaged
#include <thomas\object\component\ScriptComponent.h>
#pragma managed

#include "../Component.h"

namespace ThomasEngine
{
	[HideInInspector]
	public ref class ScriptComponent : public Component
	{
	private:

	public:
		virtual void Awake() override {};
		virtual void OnEnable() override {};
		virtual void OnDisable() override {};
		virtual void Start() override {};
		virtual void Update() override {};
		virtual void OnDrawGizmosSelected() override {};
		virtual void OnDrawGizmos() override {};
		virtual void OnCollisionEnter(GameObject^ collider) override {};

		ScriptComponent() : Component(new thomas::object::component::ScriptComponent())
		{
		}
	};
}