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
#ifdef _CORE
#ifdef _EDITOR											
#define EDITOR_LOCK()	thomas::utils::atomics::Lock lck(m_editorLock)
#else
#define EDITOR_LOCK()	{}
#endif
#define COMP_LOCK()		thomas::utils::atomics::Lock lck(m_lock)
#endif
			class Component : public Object
			{
			public:
				Component();
				virtual ~Component();	
				virtual void OnAwake() {};
				virtual void OnEnable() {};
				virtual void OnDisable() {};
				virtual void OnDestroy() {};
				virtual void Update() {};
				virtual void FixedUpdate() {};
				virtual void OnDrawGizmos() {};
				virtual void OnDrawGizmosSelected() {};

			public:
				GameObject* m_gameObject;
				/* Call from engine to set the state
				*/
				void setEnabledState(bool state);

			protected:

				bool m_enabled;

				mutable thomas::utils::atomics::SpinLock m_lock;
#ifdef _EDITOR				
				mutable thomas::utils::atomics::SpinLock m_editorLock;
#endif
			};
		}
	}
}