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
			private:
			public:
				bool initialized = false;
				GameObject* m_gameObject;
				virtual void Awake() {};
				virtual void OnEnable() { };
				virtual void OnDisable() {};
				virtual void Update() {};
				virtual void FixedUpdate() {};
				virtual void OnDrawGizmos() {};
				virtual void OnDrawGizmosSelected() {};
				virtual ~Component();


			private:
			protected:
				
			};
		}
	}
}