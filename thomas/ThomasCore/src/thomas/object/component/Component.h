#pragma once
#include "../Object.h"
#include "../../utils/atomic/Synchronization.h"
namespace thomas
{
	namespace object
	{
		class GameObject;
		namespace component
		{
#define COMP_LOCK() thomas::utils::atomics::Lock lck(m_lock)
			class Component : public Object
			{
			public:
				Component();
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
			protected:
				thomas::utils::atomics::SpinLock m_lock;
			};
		}
	}
}