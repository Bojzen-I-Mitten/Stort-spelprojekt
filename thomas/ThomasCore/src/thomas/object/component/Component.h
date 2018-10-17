#pragma once
#include "../Object.h"
namespace thomas
{
	namespace object
	{
		class GameObject;
		namespace component
		{
			class Component : public Object
			{
			public:
				virtual ~Component();	
				virtual void Awake() {};
				virtual void OnEnable() {};
				virtual void OnDisable() {};
				virtual void OnDestroy() {};
				virtual void Update() {};
				virtual void FixedUpdate() {};
				virtual void OnDrawGizmos() {};
				virtual void OnDrawGizmosSelected() {};

			public:
				bool initialized = false;
				GameObject* m_gameObject;	
			};
		}
	}
}