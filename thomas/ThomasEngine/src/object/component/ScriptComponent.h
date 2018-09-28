#pragma once


#include "../Component.h"

namespace ThomasEngine
{
	[HideInInspector]
	public ref class ScriptComponent : public Component
	{
	
	public:
		ScriptComponent();
		virtual void OnEnable()override {};
		virtual void OnDisable()override {};
		virtual void Awake()override {};
		virtual void Start()override {};
		virtual void Update()override {};
		virtual void OnDrawGizmosSelected() override {};
		virtual void OnDrawGizmos() override {};

		virtual void OnCollisionEnter(Collider^ collider) override {};
		virtual void OnCollisionStay(Collider^ collider) override {};
		virtual void OnCollisionExit(Collider^ collider) override {};

		virtual void OnTriggerEnter(Collider^ collider) override {};
		virtual void OnTriggerStay(Collider^ collider) override {};
		virtual void OnTriggerExit(Collider^ collider) override {};

	};
}